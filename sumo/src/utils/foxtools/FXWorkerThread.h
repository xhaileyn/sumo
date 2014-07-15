/****************************************************************************/
/// @file    FXWorkerThread.h
/// @author  Michael Behrisch
/// @date    2014-07-13
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2004-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#ifndef FXWorkerThread_h
#define FXWorkerThread_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <list>
#include <vector>
#include <fx.h>
#include <FXThread.h>
#include <utils/common/RandHelper.h>


class FXWorkerThread : public FXThread {

public:
    class Task {
    public:
        virtual void run(FXWorkerThread* context) = 0;
        void setIndex(const int newIndex) {
            myIndex = newIndex;
        }
        int getIndex() const {
            return myIndex;
        }
    private:
        int myIndex;
    };

    class Pool {
    public:
        Pool(int numThreads=0) : myRunningIndex(0), myNumFinished(0) {
            while (numThreads > 0) {
                new FXWorkerThread(*this);
                numThreads--;
            }
        }
        
        virtual ~Pool() {
            for (std::vector<FXWorkerThread*>::iterator it = myWorkers.begin(); it != myWorkers.end(); ++it) {
                delete *it;
            }
        }

        void addWorker(FXWorkerThread* const w) {
            myWorkers.push_back(w);
        }
        
        void add(Task* const t) {
            t->setIndex(myRunningIndex++);
            RandHelper::getRandomFrom(myWorkers)->add(t);
        }
        
        void addFinished(Task* const t) {
            myMutex.lock();
            myNumFinished++;
            myFinishedTasks.push_back(t);
            myCondition.signal();
            myMutex.unlock();
        }

        Task* popFinished() {
            Task* result = 0;
            myMutex.lock();
            if (!myFinishedTasks.empty()) {
                result = myFinishedTasks.front();
                myFinishedTasks.pop_front();
            }
            myMutex.unlock();
            return result;
        }

        std::list<Task*> getAll() {
            myMutex.lock();
            while (myNumFinished < myRunningIndex) {
                myCondition.wait(myMutex);
            }
            myMutex.unlock();
            return myFinishedTasks;
        }

    private:
        std::vector<FXWorkerThread*> myWorkers;
        FXMutex myMutex;
        FXCondition myCondition;
        std::list<Task*> myFinishedTasks;
        int myRunningIndex;
        int myNumFinished;
    };

public:
    FXWorkerThread(Pool& pool): FXThread(), myPool(pool), myStopped(false) {
        pool.addWorker(this);
        start();
    }

    virtual ~FXWorkerThread() {
        stop();
    }

    void add(Task* t) {
        myMutex.lock();
        myTasks.push_back(t);
        myCondition.signal();
        myMutex.unlock();
    }

    FXint run() {
        while (!myStopped) {
            myMutex.lock();
            while (!myStopped && myTasks.empty()) {
                myCondition.wait(myMutex);
            }
            if (myStopped) {
                myMutex.unlock();
                break;
            }
            Task* t = myTasks.front();
            myTasks.pop_front();
            myMutex.unlock();
            t->run(this);
            myPool.addFinished(t);
        }
        return 0;
    }
    
    void stop() {
        myMutex.lock();
        myStopped = true;
        myCondition.signal();
        myMutex.unlock();
        join();
    }

private:
    Pool& myPool;
    FXMutex myMutex;
    FXCondition myCondition;
    std::list<Task*> myTasks;
    bool myStopped;
};


#endif
