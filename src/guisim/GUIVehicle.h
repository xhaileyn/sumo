#ifndef GUIVehicle_h
#define GUIVehicle_h
//---------------------------------------------------------------------------//
//                        GUIVehicle.h -
//  A MSVehicle extended by some values for usage within the gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.8  2003/07/30 08:54:14  dkrajzew
// the network is capable to display the networks state, now
//
// Revision 1.7  2003/06/05 11:40:28  dkrajzew
// class templates applied; documentation added
//
// Revision 1.6  2003/06/05 06:29:50  dkrajzew
// first tries to build under linux: warnings removed; moc-files included Makefiles added
//
// Revision 1.5  2003/05/20 09:26:57  dkrajzew
// data retrieval for new views added
//
// Revision 1.4  2003/04/14 08:27:18  dkrajzew
// new globject concept implemented
//
// Revision 1.3  2003/03/20 16:19:28  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.2  2003/02/07 10:39:17  dkrajzew
// updated
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <gui/GUIGlObject.h>
#include <microsim/MSVehicle.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUISUMOAbstractView;
class QGLObjectPopupMenu;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * A visualisable MSVehicle. Extended by the possibility to retrieve names
 * of all available vehicles (static) and the possibility to retrieve the
 * color of the vehicle which is available in different forms allowing an
 * easier recognition of done actions such as lane changing.
 */
class GUIVehicle :
                public MSVehicle,
                public GUIGlObject {
public:
    /// destructor
    ~GUIVehicle();

    /// returns the popup-menu for vehicles
    QGLObjectPopupMenu *getPopUpMenu(GUIApplicationWindow &app,
        GUISUMOAbstractView &parent);

    GUIParameterTableWindow *getParameterWindow(
        GUIApplicationWindow &app, GUISUMOAbstractView &parent);

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    /// returns the id of the object as known to microsim
    std::string microsimID() const;

    /// retunrs the names of all available vehicles
    static std::vector<std::string> getNames();

    /// returns the color of the vehicle defined in the xml-description
    const float *getDefinedColor() const;

    /** returns a random color based on the vehicle's name
        (should stay the same across simulations */
    const float *getRandomColor1() const;

    /** retunrs a random color
        (this second random color is fully randomly computed) */
    const float *getRandomColor2() const;

    /** returns a color that describes how long ago the vehicle has
        changed the lane (is white after a lane change and becomes darker
        with each timestep */
    int getPassedColor() const;

    /** returns white if the vehicle has changed the lane in the current step,
        othewise dark grey */
    const float *getLaneChangeColor2() const;

    /** @brief returns the number of steps waited
        A vehicle is meant to be "waiting" when it's speed is less than 0.1
        It is only computed for "critical" vehicles
        The method return a size_t, now, as we assume a vehicle will not wait for
        longer than about 50 hours which still fits into a size_t when the simulation
        runs in ms */
    size_t getWaitingTime() const;

    /** @brief Returns the next "periodical" vehicle with the same route
        We have to duplicate the vehicle if a further has to be emitted with
        the same settings */
    virtual MSVehicle *getNextPeriodical() const;


//    virtual size_t getTableParameterNo() const;

    /// GUINet is allowed to build vehicles
    friend class GUINet;
/*
    double getTableParameter(size_t pos) const;



    const char * const getTableItem(size_t pos) const;
*/
//    void fillTableParameter(double *parameter) const;
	bool active() const;

	void setRemoved();

    size_t getRepetitionNo() const;
    size_t getPeriod() const;
    size_t getLastLaneChangeOffset() const;
    size_t getRealDepartTime() const;
    size_t getDesiredDepart() const;



protected:
    /// Use this constructor only.
    GUIVehicle( GUIGlObjectStorage &idStorage,
        std::string id, MSRoute* route, MSNet::Time departTime,
        const MSVehicleType* type, size_t noMeanData,
        int repNo, int repOffset, float *defColor);
/*
    TableType getTableType(size_t pos) const;

    const char *getTableBeginValue(size_t pos) const;
*/

private:
    /// the color read from the XML-description
    float _definedColor[3];

    /// random color #1 (build from name)
    float _randomColor1[3];

    /// random color #2 (completely random)
    float _randomColor2[3];

    /// white
    static float _laneChangeColor1[3];

    /// dark grey
    static float _laneChangeColor2[3];

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIVehicle.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

