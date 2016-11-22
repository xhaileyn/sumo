/****************************************************************************/
/// @file    GNECrossingFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
/// @version $Id$
///
/// The Widget for add Crossing elements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNECrossingFrame_h
#define GNECrossingFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEFrame.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNENetElement;
class GNECrossing;
class GNEJunction;
class GNEEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECrossingFrame
 * The Widget for setting default parameters of Crossing elements
 */
class GNECrossingFrame : public GNEFrame {
    /// @brief FOX-declaration
    FXDECLARE(GNECrossingFrame)

public:

    // ===========================================================================
    // class edgesSelector
    // ===========================================================================

    class edgesSelector : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNECrossingFrame::edgesSelector)

    public:
        /// @brief constructor
        edgesSelector(FXComposite* parent, GNECrossingFrame* crossingFrameParent);

        /// @brief destructor
        ~edgesSelector();

        /// @brief get IDs of selected edges
        std::vector<std::string> getEdgeIDSSelected() const;

        /// @brief get vector of GNEEdges selected
        std::vector<GNEEdge*> getGNEEdgesSelected() const;

        /// @brief get current junction
        GNEJunction* getCurrentJunction() const;

        /// @brief clear list of edges
        void clearList();

        /// @brief mark as selected a edge of the list
        bool markEdge(const std::string &edgeID);

        /// @brief enable edgeSelector 
        void enableEdgeSelector(GNEJunction *currentJunction);

        /// @brief disable edgeSelector 
        void disableEdgeSelector();

        /// @brief Update use selectedEdges
        void updateUseSelectedEdges();

        /// @brief get status of checkBox UseSelectedEdges
        bool isUseSelectedEdgesEnable() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user trigger checkBox of useSelectedEdges
        long onCmdShowOnlySelectedEdges(FXObject*, FXSelector, void*);

        /// @brief called when user type in search box
        long onCmdTypeInSearchBox(FXObject*, FXSelector, void*);

        /// @brief called when user select a edge of the list
        long onCmdSelectEdge(FXObject*, FXSelector, void*);

        /// @brief called when clear selection button is pressed
        long onCmdClearSelection(FXObject*, FXSelector, void*);

        /// @brief called when invert selection button is pressed
        long onCmdInvertSelection(FXObject*, FXSelector, void*);

        /// @brief called when user click over TextField for edge search
        long onCmdClickEdgeSearchFocusIn(FXObject*, FXSelector, void*);

        /// @brief called when user leave the TexField for edge search
        long onCmdClickEdgeSearchFocusOut(FXObject*, FXSelector, void*);

        /// @brief called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        edgesSelector() {}
        
        /// @brief filter list of edges with the search criterium
        void filterListOfEdges(std::string search = "");

    private:
        /// @brief pointer to GNECrossingFrame parent
        GNECrossingFrame *myCrossingFrameParent;

        /// @brief CheckBox for selected edges
        FXMenuCheck* myShowOnlySelectedEdges;

        /// @brief List of edgesSelector
        FXList* myList;

        /// @brief text field for search edge IDs
        FXTextField* myEdgesSearch;

        /// @brief button for help
        FXButton* helpEdges;

        /// @brief button for clear selection
        FXButton* myClearEdgesSelection;

        /// @brief button for invert selection
        FXButton* myInvertEdgesSelection;

        /// @brief current Junction
        GNEJunction *myCurrentJunction;

        /// @brief current selected edges
        std::vector<GNEEdge*> myCurrentSelectedEdges;
    };

    // ===========================================================================
    // class crossingParameters
    // ===========================================================================

    class crossingParameters : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNECrossingFrame::crossingParameters)

    public:
        /// @brief constructor
        crossingParameters(FXComposite* parent, GNECrossingFrame::edgesSelector *es);

        /// @brief destructor
        ~crossingParameters();

        /// @brief enable crossing parameters and set the default value of parameters
        void enableCrossingParameters();

        /// @brief disable crossing parameters and clear parameters
        void disableCrossingParameters();

        /// @brief update edges
        void updateEdges();

        /// @brief check if currently the crossingParameters is enabled
        bool isCrossingParametersEnabled() const;

        /// @brief get crossing NBedges
        std::vector<NBEdge*> getCrossingEdges() const;

        /// @brief get crossing priority
        bool getCrossingPriority() const;

        /// @brief get crossing width
        SUMOReal getCrossingWidth() const;

        /// @brief check if current parameters are valid
        bool isCurrentParametersValid() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user set a value 
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        crossingParameters() {}

    private:
        /// @brief pointer to edge selector
        GNECrossingFrame::edgesSelector *myEdgeSelector;

        /// @brief FXMatrix for place attributes
        FXMatrix *myAttributesMatrix;

        /// @brief Label for edges
        FXLabel *myCrossingEdgesLabel;

        /// @brief TextField for edges
        FXTextField *myCrossingEdges;

        /// @brief Label for Priority
        FXLabel *myCrossingPriorityLabel;

        /// @brief CheckBox for Priority
        FXMenuCheck *myCrossingPriority;

        /// @brief Label for width
        FXLabel *myCrossingWidthLabel;

        /// @brief TextField for width
        FXTextField *myCrossingWidth;

        /// @brief button for help
        FXButton* myHelpAdditional;

        /// @brief flag to check if current parameters are valid
        bool myCurrentParametersValid;
    };


    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNECrossingFrame(FXHorizontalFrame *horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNECrossingFrame();


    /**@brief add Crossing element
     * @param[in] netElement clicked netElement. if user dind't clicked over a GNENetElement in view, netElement will be NULL
     * @return true if a GNECrossing was added, false in other case
     */
    bool addCrossing(GNENetElement* netElement);

    /**@brief remove an Crossing element previously added
     * @param[in] Crossing element to erase
     */
    void removeCrossing(GNECrossing* Crossing);

    /// @brief enable or disable button create edges
    void setCreateCrossingButton(bool value);

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user press the button create edge
    long onCmdCreateCrossing(FXObject*, FXSelector, void*);
    /// @}

    /// @brief get list of selecte id's in string format
    static std::string getIdsSelected(const FXList* list);

protected:
    /// @brief FOX needs this
    GNECrossingFrame() {}

    // get Crossing parameters
    GNECrossingFrame::crossingParameters *getCrossingParameters() const;

private:
    /// @brief edge selector
    GNECrossingFrame::edgesSelector *myEdgeSelector;

    /// @brief crossing parameters
    GNECrossingFrame::crossingParameters *myCrossingParameters;

    /// @brief groupbox for the junction label
    FXGroupBox* myGroupBoxLabel;

    /// @brief Label for current Junction
    FXLabel *myCurrentJunctionLabel;

    /// @brief groupbox for buttons
    FXGroupBox* myGroupBoxButtons;

    /// @field FXButton for create Crossing
    FXButton *myCreateCrossingButton;
};


#endif

/****************************************************************************/