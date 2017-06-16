#!/usr/bin/env python
"""
@file    tlsCycleAdaptation.py
@author  Yun-Pang Floetteroed
@date    2017-05-10
@version $Id: tlsCycleAdaptation.py

- The Webster's equation is used to optimize the cycle length
  and the green times of the traffic lights in a sumo network
  with a given route file.

- Traffic lights without traffic flows will not be optimized.
- PCE is used instead of the number of vehicles.

- If a lane group has green times in more than one phase, 
  the respective flows will be equally divided into the corresponding
  phases for calculating the green splits. 

- If the critial flow or the sum of the critial flows is larger than 1,
 the optimal cycle length will be set to 120 sec. 

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import subprocess
import xml.etree.cElementTree as ET
import optparse
import collections

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
    import sumolib
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

from sumolib.miscutils import Statistics
from sumolib.output import parse_fast
from operator import itemgetter
from collections import namedtuple
from sumolib.net import connection

def get_options(args=None):
    optParser = optparse.OptionParser()
    optParser.add_option("-n", "--net-file", dest="netfile",
                         help="define the net file (mandatory)")
    optParser.add_option("-o", "--output-file", dest="outfile",
                         default="tlsAdaptation.add.xml", help="define the output filename")
    optParser.add_option("-r", "--route-files", dest="routefiles",
                         help="define the route file seperated by comma(mandatory)")
    optParser.add_option("-b", "--begin", dest="begin", type="int",
                         default = 0, help="begin time of the optmization period with unit second")
    optParser.add_option("-y", "--yellow-time", dest="yellowtime", type="int",
                         default=4, help="yellow time")
    optParser.add_option("-a", "--all-red", dest="allred", type="int",
                         default=0, help="all-red time")
    optParser.add_option("-l", "--lost-time", dest="losttime", type="int",
                         default=4, help="lost time for start-up and clearance in each phase")
    optParser.add_option("-g", "--min-green", dest="mingreen", type="int",
                         default=4, help=" minimal green time when there is no traffic volume")
    optParser.add_option("-c", "--min-cycle", dest="mincycle", type="int",
                         default=20, help=" minimal cycle length")
    optParser.add_option("-C", "--max-cycle", dest="maxcycle", type="int",
                         default=120, help=" maximal cycle length")
    optParser.add_option("-e", "--existing-cycle", dest="existcycle", action="store_true",
                         default=False, help=" use the existing cycle length")
    optParser.add_option("-H", "--saturation-headway", dest="satheadway", type="float",
                         default=2, help=" saturation headway in seconds for calcuating hourly saturation flows")
    optParser.add_option("-R", "--restrict-cyclelength", dest="restrict", action="store_true",
                         default=False, help=" restrict the max. cycle length as the given one")
    optParser.add_option("-v", "--verbose", dest="verbose", action="store_true",
                         default=False, help="tell me what you are doing")
    (options, args) = optParser.parse_args(args=args)
    
    if not options.netfile or not options.routefiles:
        optParser.print_help()
        sys.exit()

    return options
    
def getFlows(net, routeFiles, tlsList, begin, verbose):
    tlsFlowsMap = {}
    end = begin + 3600
    for tls in tlsList:
        tlsFlowsMap[tls._id] = collections.defaultdict(lambda:collections.defaultdict(int))
    for file in routeFiles.split(','):
        if verbose:
            print ("route file:%s" %file)
        for veh in sumolib.output.parse(file, 'vehicle'):
            route = veh.route[0]
            if int(veh.depart) < end and int(veh.depart) >= begin:
                for tls in tlsList:
                    connsList = tls.getConnections()
                    for c in sorted(connsList, key=lambda connsList: connsList[2]): # c: [[inLane, outLane, linkNo],[],..]
                        subRoute = c[0]._edge._id + ' ' + c[1]._edge._id
                        edgeList = route.edges.split()
                        if c[0]._edge._id in edgeList:
                            beginindex = edgeList.index(c[0]._edge._id)
                            if beginindex < 0:
                                print ("negtive beginindex: %s" %beginindex)
                            elif beginindex < len(edgeList)-1 and edgeList[beginindex+1] == c[1]._edge._id:
                                pce = 1.
                                if veh.type == "bicycle":
                                    pce = 0.2
                                elif veh.type in ["moped", "motorcycle"]:
                                    pce = 0.5
                                elif veh.type in ["truck", "trailer", "bus", "coach"]:
                                    pce = 3.5
                                tlsFlowsMap[tls._id][subRoute][c[2]] += pce
        
    # remove the doubled counts
    connFlowsMap = {}
    for t in tlsList:
        connFlowsMap[t._id] = {}
        for subRoute in tlsFlowsMap[t._id]:
            totalConns = len(tlsFlowsMap[t._id][subRoute])
            for conn in tlsFlowsMap[t._id][subRoute]:
                tlsFlowsMap[t._id][subRoute][conn] /= totalConns
                connFlowsMap[t._id][conn] = tlsFlowsMap[t._id][subRoute][conn]
                    
        # remove the redundant connection flows
        connFlowsMap = removeRedudantFlows(t, connFlowsMap)

    return connFlowsMap
    
def getEffectiveTlsList(tlsList, connFlowsMap, verbose):
    effectiveTlsList = []
    for tl in tlsList:
        for conn in connFlowsMap[tl._id]:
            if connFlowsMap[tl._id][conn] > 0:
                effectiveTlsList.append(tl)
                break
    return effectiveTlsList

def removeRedudantFlows(t, connFlowsMap):
    # if two or more intesections share the laen-lane connection indices together,
    # the redudant connection flows will set to zero.
    connsList = t.getConnections()
    connsList = sorted(connsList, key=lambda connsList: connsList[2])
    redudantConnsList = []
    identical = True
    for c1 in connsList:
        for c2 in connsList:
            if c1[2] != c2[2]:
                if c1[1]._edge == c2[0]._edge:
                    indentical = indenticalCheck(c1[0]._edge, c2[0]._edge._incoming, identical)
                    if identical:
                        for toEdge in c2[0]._edge._outgoing:
                            for c in c2[0]._edge._outgoing[toEdge]:
                                if c._tlLink not in redudantConnsList:
                                    redudantConnsList.append(c._tlLink)
                    else:
                        for conn_1 in c1[0]._edge._outgoing[c2[0]._edge]:
                            if conn_1._direction == 's':
                                for toEdge in c2[0]._edge._outgoing:
                                    for conn_2 in c2[0]._edge._outgoing[toEdge]:
                                        if conn_2._tlLink not in redudantConnsList:
                                            redudantConnsList.append(conn_2._tlLink)
    for conn in redudantConnsList:
        if conn in connFlowsMap[t._id]:
            connFlowsMap[t._id][conn] = 0.
    return connFlowsMap
    
def indenticalCheck(e1, incomingLinks, identical):
    for i in incomingLinks:
        if i != e1:
            identical = False
            break
    return identical
         
def getLaneGroupFlows(tl, connFlowsMap, phases):
    connsList = tl.getConnections()
    groupFlowsMap = {}     #i(phase): duration, laneGroup1, laneGroup2, ...
    connsList = sorted(connsList, key=lambda connsList: connsList[2])
        
    # check if there are shared lane groups, i.e. some lane groups have only "g" (no "G")
    ownGreenConnsList = []
    for i, p in enumerate(phases):
        totalConns = len(p[0])
        for j, control in enumerate(p[0]):
            if control == "G" and j not in ownGreenConnsList:
                ownGreenConnsList.append(j)
    yellowRedTime = 0
    greenTime = 0
    currentLength = 0
    phaseLaneIndexMap = collections.defaultdict(list)
    for i, p in enumerate(phases):
        currentLength += p[1]
        if 'G' in p[0]:
            greenTime += p[1]
            groupFlowsMap[i] = [p[1]]
            groupFlows = 0
            laneIndexList = []
            for j, control in enumerate(p[0]):
                inEdge = connsList[j][0]._edge._id
                if j == 0:
                    exEdge = inEdge
                if (inEdge == exEdge and control =='G') or (inEdge == exEdge and control == 'g' and j not in ownGreenConnsList):
                    if j in connFlowsMap[tl._id]:
                        groupFlows += connFlowsMap[tl._id][j]
                    if connsList[j][0].getIndex() not in laneIndexList:
                        laneIndexList.append(connsList[j][0].getIndex())
                        
                if exEdge != inEdge or j == len(p[0])-1:
                    if laneIndexList:
                        phaseLaneIndexMap[i].append(laneIndexList)
                        groupFlowsMap[i].append(groupFlows)
                        
                    laneIndexList = []
                    groupFlows = 0
                    if control == "G":
                        if j in connFlowsMap[tl._id]:
                            groupFlows = connFlowsMap[tl._id][j]
                            if connsList[j][0].getIndex() not in laneIndexList:
                                laneIndexList.append(connsList[j][0].getIndex())
                exEdge = inEdge
        elif 'G' not in p[0] and 'g' in p[0] and 'y' not in p[0] and 'r' not in p[0]:
            print ("Check: only g for all connections:%s in phase %s" %(tl._id, i))
        elif ('G' not in p[0] and 'g' not in p[0]) or ('G' not in p[0] and 'y' in p[0] and 'r' in p[0]):
            yellowRedTime += int(p[1])
        if options.verbose and i in groupFlowsMap:
            print ("phase: %s" %i)
            print ("group flows: %s" %groupFlowsMap[i])
            print ("The used lanes: %s" % phaseLaneIndexMap[i])
    if options.verbose:
        print ("the current cycle length:%s sec" %currentLength)
    return groupFlowsMap, phaseLaneIndexMap, currentLength

def optimizeGreenTime(groupFlowsMap, phaseLaneIndexMap, currentLength, options):
    lostTime = len(groupFlowsMap)*options.losttime + options.allred
    satFlows = 3600./options.satheadway
    # calculate the critial flow ratios and the respective sum
    critialFlowRateMap = {}
    for i in groupFlowsMap:   # [duration. groupFlow1, groupFlow2...]
        critialFlowRateMap[i] = 0.
        maxFlow = 0
        index = None
        if len(groupFlowsMap[i][1:]) > 0:
            for j, f in enumerate(groupFlowsMap[i][1:]):
                if f >= maxFlow:
                    maxFlow = f
                    index = j
            critialFlowRateMap[i] = (maxFlow/float((len(phaseLaneIndexMap[i][index]))))/satFlows
        else:
            critialFlowRateMap[i] = 0.
    sumCritialFlows = sum(critialFlowRateMap.values())
    
    if options.existcycle:
        optCycle = currentLength
    elif sumCritialFlows >= 1.:
        optCycle = options.maxcycle
        if options.verbose:
            print ("Warning: the sum of the critial flows >= 1:%s" %sumCritialFlows)
    else:
        optCycle = int(round((1.5*lostTime + 5.)/(1. - sumCritialFlows)))

    if not options.existcycle and optCycle < options.mincycle:
        optCycle = options.mincycle
    elif not options.existcycle and optCycle > options.maxcycle:
        optCycle = options.maxcycle

    effGreenTime = optCycle - lostTime
    totalLength = lostTime
    minGreenPhasesList = []
    adjustGreenTimes = 0
    totalGreenTimes = 0
    subtotalGreenTimes = 0
    for i in critialFlowRateMap:
        groupFlowsMap[i][0] = effGreenTime * (critialFlowRateMap[i]/sum(critialFlowRateMap.values())) - options.yellowtime + options.losttime
        groupFlowsMap[i][0] = int(round(groupFlowsMap[i][0]))
        totalGreenTimes += groupFlowsMap[i][0]
        if groupFlowsMap[i][0] < options.mingreen:
            groupFlowsMap[i][0] = options.mingreen
            minGreenPhasesList.append(i)
        else:
            subtotalGreenTimes += groupFlowsMap[i][0]
        totalLength += groupFlowsMap[i][0]
        
    # adjust the green times if minmal green times are applied for keeping the defined maximal cycle length.
    if minGreenPhasesList and totalLength > options.maxcycle and options.restrict:
        if options.verbose:
            print ("Re-allocate the green splits!")
        adjustGreenTimes = totalGreenTimes - len(minGreenPhasesList) * options.mingreen
        for i in groupFlowsMap:
            if i not in minGreenPhasesList:
                groupFlowsMap[i][0] = int((groupFlowsMap[i][0]/float(subtotalGreenTimes))*adjustGreenTimes)

    if options.verbose:
        totalLength = lostTime
        for i in groupFlowsMap:
            totalLength += groupFlowsMap[i][0]
            print ("Green time for phase %s: %s" %(i, groupFlowsMap[i][0]))
        print ("the optimal cycle lenth:%s" %totalLength)

    return groupFlowsMap
    
def main(options):
    net = sumolib.net.readNet(options.netfile, withPrograms=True)
    tlsList = net.getTrafficLights()
    nodesList = net.getNodes()
    if options.verbose:
        print("the total number of tls: %s" % len(tlsList))
    print ("Begin time:%s" %options.begin) 
    # get traffic flows for each connection at each TL
    connFlowsMap = getFlows(net, options.routefiles, tlsList, options.begin, options.verbose)
    
    # remove the tls where no traffic volumes exist
    effectiveTlsList = getEffectiveTlsList(tlsList, connFlowsMap, options.verbose)
    
    with open(options.outfile, 'w') as outf:
        outf.write('<?xml version="1.0" encoding="UTF-8"?>\n')
        outf.write('<additional>\n')

        if len(effectiveTlsList) > 0:
            for tl in effectiveTlsList:
                if options.verbose:
                    print ("tl-logic ID: %s" %tl._id)
                programs = tl.getPrograms()
                for pro in programs:
                    phases = programs[pro].getPhases()

                    # get the connection flows and group flows
                    groupFlowsMap, phaseLaneIndexMap, currentLength = getLaneGroupFlows(tl, connFlowsMap, phases)
                    
                    # optimize the cycle length and green times
                    groupFlowsMap = optimizeGreenTime(groupFlowsMap, phaseLaneIndexMap, currentLength, options)
                
                # write output
                    outf.write('    <tlLogic id="%s" type="%s" programID="%s" offset="%.2f">\n' %
                                    (tl._id, programs[pro]._type, "a", programs[pro]._offset))
                                    
                    phases = programs[pro].getPhases()
                    for i, p in enumerate(phases):
                        duration = p[1]
                        if i in groupFlowsMap:
                            duration = groupFlowsMap[i][0]
                        outf.write('        <phase duration="%s" state="%s"/>\n' %(duration, p[0]))
                    outf.write('    </tlLogic>\n')
        else:
            print("There are no flows at the given intersections. No green time optimization is done.")
        outf.write('</additional>\n')

if __name__ == "__main__":
    options = get_options(sys.argv)
    main(options)