/* osgCompute - Copyright (C) 2008-2009 SVT Group
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesse General Public License for more details.
 *
 * The full license is in LICENSE file included with this distribution.
*/
#include <memory.h>
#include <osg/Notify>
#include <osgCuda/Buffer>
#include <osgCuda/Context>
#include <osgCompute/Module>

//------------------------------------------------------------------------------
extern "C"
void swapEndianness( unsigned int numBlocks, unsigned int numThreads, void* bytes );


/**
*/
class SwapModule : public osgCompute::Module
{
public:
    SwapModule() : osgCompute::Module() {clearLocal();}

    META_Object( , SwapModule )
	virtual bool init();
	virtual void launch( const osgCompute::Context& context ) const;

    inline void setBuffer( osgCompute::Buffer* buffer ) { _buffer = buffer; }

	virtual void clear() { clearLocal(); osgCompute::Module::clear(); }
protected:
    virtual ~SwapModule() { clearLocal(); }
    void clearLocal() { _buffer = NULL; }

    unsigned int                                     _numThreads;
    unsigned int                                     _numBlocks;
	osg::ref_ptr<osgCompute::Buffer>                 _buffer;

private:
    SwapModule(const SwapModule&, const osg::CopyOp& ) {}
    inline SwapModule &operator=(const SwapModule &) { return *this; }
};

//------------------------------------------------------------------------------
void SwapModule::launch( const osgCompute::Context& ctx ) const
{
    swapEndianness( _numBlocks, _numThreads, _buffer->map( ctx ) );
}

//------------------------------------------------------------------------------
bool SwapModule::init()
{
    if( !_buffer )
        return false;

    _numThreads = 1;
    _numBlocks = _buffer->getDimension(0) / _numThreads;

    return osgCompute::Module::init();
}

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    osg::setNotifyLevel( osg::INFO );

	// You can use modules and buffers in the update cycle or everywhere
	// you want. But please make sure that the context is still active at
	// computation time if you use osgCuda::Geometry or osgCuda::Texture objects!!!

	///////////////////
	// BEFORE LAUNCH //
	///////////////////
    unsigned int bigEndians[] = { 0x3faff7b4, 0x32332323, 0xffccaadd, 0xaaaacccc };
    unsigned int numEndians = sizeof(bigEndians)/sizeof(unsigned int);

	osg::notify(osg::INFO)<<"Before conversion: "<<std::endl;
	for( unsigned int v=0; v<numEndians; ++v )
		osg::notify(osg::INFO)<<std::hex<< bigEndians[v] <<std::endl;

	////////////////////
	// CREATE CONTEXT //
	////////////////////
	// You have to provide a context to run on. If this module
	// is attached to the scene graph then you do not need to
	// prepare a context, the computation node will take care of it.
    osg::ref_ptr<osgCompute::Context> context = new osgCuda::Context;
    if( !context.valid() )
        return -1;
	// The context must have a device id.
	// The default is set to device 0. Change
	// it with the following command.
	// context->setDevice( 0 );

    // activate context. Initialize it if not done so far
	context->init();
    context->apply();

    // create a buffer
    osg::ref_ptr<osgCuda::Buffer> buffer = new osgCuda::Buffer;
	buffer->setElementSize( sizeof(unsigned int) );
    buffer->setDimension(0, numEndians);
    buffer->init();


	///////////////////
	// LAUNCH MODULE //
	///////////////////
    osg::ref_ptr<SwapModule> module = new SwapModule;
    if( !module.valid() )
        return -1;

    module->setBuffer( buffer.get() );
    module->init();

	// instead of attaching a osg::Array you can map the buffer to the
	// CPU memory and fill it directly. The TARGET specifier in MAP_HOST_TARGET
	// tells osgCompute that the buffer is updated on the CPU. This has an effect
	// on later mappings of the GPU memory (e.g. MAP_DEVICE): before a pointer
	// is returned the CPU data is copied to the GPU memory.
    unsigned int* bufferPtr = (unsigned int*)buffer->map( *context, osgCompute::MAP_HOST_TARGET );
    memcpy( bufferPtr, bigEndians, sizeof(bigEndians) );

    module->launch( *context );

	//////////////////
	// AFTER LAUNCH //
	//////////////////
    bufferPtr = (unsigned int*)buffer->map( *context, osgCompute::MAP_HOST_SOURCE );
    osg::notify(osg::INFO)<<std::endl<<"After conversion: "<<std::endl;
    for( unsigned int v=0; v<buffer->getDimension(0); ++v )
        osg::notify(osg::INFO)<<std::hex<< bufferPtr[v] <<std::endl;

    return 0;
}
