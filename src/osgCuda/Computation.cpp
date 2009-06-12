#include <osg/NodeVisitor>
#include <osg/OperationThread>
#include <osgUtil/CullVisitor>
#include "osgCuda/Computation"

namespace osgCuda
{ 
    /////////////////////////////////////////////////////////////////////////////////////////////////
    // PUBLIC FUNCTIONS /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    //------------------------------------------------------------------------------
    Computation::Computation() 
        :   osgCompute::Computation()
    { 
        clearLocal(); 
    }

    //------------------------------------------------------------------------------
    void Computation::clear() 
    { 
        clearLocal();
        osgCompute::Computation::clear(); 
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    // PROTECTED FUNCTIONS //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    //------------------------------------------------------------------------------
    void Computation::clearLocal() 
    {
    }
}
