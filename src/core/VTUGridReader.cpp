/*---------------------------------------------------------------------------*\
 *
 *  mimmo
 *
 *  Copyright (C) 2015-2017 OPTIMAD engineering Srl
 *
 *  -------------------------------------------------------------------------
 *  License
 *  This file is part of mimmo.
 *
 *  mimmo is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License v3 (LGPL)
 *  as published by the Free Software Foundation.
 *
 *  mimmo is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with mimmo. If not, see <http://www.gnu.org/licenses/>.
 *
\*---------------------------------------------------------------------------*/

#include "VTUGridReader.hpp"

using namespace std;
using namespace bitpit;

namespace mimmo{

/*!
 * Base Constructor
 */
VTUAbsorbStreamer::VTUAbsorbStreamer() : VTKBaseStreamer(){}

/*!
 * Base Destructor
 */
VTUAbsorbStreamer::~VTUAbsorbStreamer(){}

/*!
 * Absorber of VTU mesh data. Reimplemented from bitpit::VTKBaseStreamer class
 * \param[in] stream    stream to read from
 * \param[in] name      name of the geometry field
 * \param[in] format    ASCII or APPENDED
 * \param[in] entries   number of entries for data container
 * \param[in] components UNUSED parameter
 */
void VTUAbsorbStreamer::absorbData(std::fstream &stream, std::string name, bitpit::VTKFormat format, 
                                 uint64_t entries, uint8_t components)
{
    bitpit::VTKBaseStreamer::absorbData(stream, name, format, entries, components);
}

/*!
 * Base Constructor
 */
VTUGridStreamer::VTUGridStreamer():VTUAbsorbStreamer(){}

/*!
 * Base Destructor
 */
VTUGridStreamer::~VTUGridStreamer(){}

/*!
 * Absorber of VTU mesh data. Reimplemented from bitpit::VTKBaseStreamer class
 * \param[in] stream    stream to read from
 * \param[in] name      name of the geometry field
 * \param[in] format    ASCII or APPENDED
 * \param[in] entries   number of entries for data container
 * \param[in] components UNUSED parameter
 */
void VTUGridStreamer::absorbData(std::fstream &stream, std::string name, bitpit::VTKFormat format, 
                                 uint64_t entries, uint8_t components)
{
    std::size_t sizeData = std::size_t(entries/components);
    if (name == "Points") {
        points.resize(sizeData);
        for (auto & p :points) {
            for(auto &pval : p){
                if(format == bitpit::VTKFormat::ASCII)  genericIO::absorbASCII(stream,pval);
                else                                    genericIO::absorbBINARY(stream, pval);
            }
        }
    } else if (name == "offsets") {
        offsets.resize(sizeData);
        for (auto & voffset : offsets) {
            if(format == bitpit::VTKFormat::ASCII)  genericIO::absorbASCII(stream,voffset);
            else                                    genericIO::absorbBINARY(stream, voffset);
        }
    } else if (name == "types") {
        types.resize(sizeData);
        for (auto & vtype : types) {

            int dumType;
            if(format == bitpit::VTKFormat::ASCII)  genericIO::absorbASCII(stream,dumType);
            else                                    genericIO::absorbBINARY(stream, dumType);
            
            switch (dumType)  {
                case 1:
                    vtype = bitpit::ElementType::VERTEX;
                    break;
                    
                case 3:
                    vtype = bitpit::ElementType::LINE;
                    break;
                    
                case 5:
                    vtype = bitpit::ElementType::TRIANGLE;
                    break;
                    
                case 7:
                    vtype = bitpit::ElementType::POLYGON;
                    break;
                    
                case 8:
                    vtype = bitpit::ElementType::PIXEL;
                    break;
                    
                case 9:
                    vtype = bitpit::ElementType::QUAD;
                    break;
                    
                case 10:
                    vtype = bitpit::ElementType::TETRA;
                    break;
                    
                case 11:
                    vtype = bitpit::ElementType::VOXEL;
                    break;
                    
                case 12:
                    vtype = bitpit::ElementType::HEXAHEDRON;
                    break;
                    
                case 13:
                    vtype = bitpit::ElementType::WEDGE;
                    break;
                    
                case 14:
                    vtype = bitpit::ElementType::PYRAMID;
                    break;
                    
                case 42:
                    vtype = bitpit::ElementType::POLYHEDRON;
                    break;
                    
                default:
                    vtype = bitpit::ElementType::UNDEFINED;
                    break;
                    
            }
        }
    } else if (name == "connectivity") {
        connectivitylist.resize(sizeData);
        for (auto & vconn : connectivitylist) {
            if(format == bitpit::VTKFormat::ASCII)  genericIO::absorbASCII(stream,vconn);
            else                                    genericIO::absorbBINARY(stream, vconn);
        }
    } else if (name == "faces") {
        faces.resize(sizeData);
        for (auto & vface : faces) {
            if(format == bitpit::VTKFormat::ASCII)  genericIO::absorbASCII(stream,vface);
            else                                    genericIO::absorbBINARY(stream, vface);
        }
    } else if (name == "faceoffsets") {
        faceoffsets.resize(sizeData);
        for (auto & vfoffset : faceoffsets) {
            if(format == bitpit::VTKFormat::ASCII)  genericIO::absorbASCII(stream,vfoffset);
            else                                    genericIO::absorbBINARY(stream, vfoffset);
        }
    } else if (name == "cellIndex") {
        cellsID.resize(sizeData);
        for (auto & vcid : cellsID) {
            if(format == bitpit::VTKFormat::ASCII)  genericIO::absorbASCII(stream,vcid);
            else                                    genericIO::absorbBINARY(stream, vcid);
        }
    } else if (name == "PID") {
        pids.resize(sizeData);
        for (auto & pid : pids) {
            if(format == bitpit::VTKFormat::ASCII)  genericIO::absorbASCII(stream,pid);
            else                                    genericIO::absorbBINARY(stream, pid);
        }
    } else if (name == "vertexIndex") {
        pointsID.resize(sizeData);
        for (auto & vpid : pointsID) {
            if(format == bitpit::VTKFormat::ASCII)  genericIO::absorbASCII(stream,vpid);
            else                                    genericIO::absorbBINARY(stream, vpid);
        }
    }
}

/*!
 * Use streamer absorbed data, if any, to fill vertices and cells info of target bitpit::PatchKernel container, 
 * passed externally. Please notice, container must be empty.
 * \param[in] patch    container for mesh
 */
void VTUGridStreamer::decodeRawData(bitpit::PatchKernel & patch)
{

    //time to check reading result.
    std::size_t nVertices, nCells;
    nVertices = points.size();
    nCells = offsets.size();
    if(nVertices == 0){
        throw std::runtime_error("Error VTUGridStreamer : no point coordinates detected while reading *.vtu file.");
    }
    if(nCells == 0 || types.empty() || connectivitylist.size() < nCells ){
        throw std::runtime_error("Error VTUGridStreamer : no valid connectivity/offsets/types info detected while reading *.vtu file.");
    }
    
    patch.reserveVertices(nVertices);
    patch.reserveCells(nCells);
    
    //reading mesh nodes and store it in vertices.
    //check labels if any;
    bool checkPointsID;
    {
        std::unordered_set<long> checkSet(pointsID.begin(), pointsID.end());
        checkPointsID = ( checkSet.size() == nVertices);
    }
    //insert points and recover local/global map of vertices;
    std::unordered_map<int, long> mapVert;
    int counter = 0;
    long idV=0;
    for(const auto & p : points){
        idV = bitpit::Vertex::NULL_ID;
        if(checkPointsID) idV = pointsID[counter];
        bitpit::PatchKernel::VertexIterator it = patch.addVertex(p, idV);
        mapVert[counter] = (*it).getId();
        ++counter;
    }

    //reading mesh connectivity by offsets and store it in cells.
    //check cell labels if any;
    bool checkCellsID, checkFaceOffset, checkPID;
    {
        std::unordered_set<long> checkSet(cellsID.begin(), cellsID.end());
        checkCellsID = ( checkSet.size() == nCells);
        checkFaceOffset = ( (faceoffsets.size() == nCells) && (faces.size()>= nCells) );
        checkPID = (pids.size() == nCells);
    }
    //insert points;
    counter = 0;
    long idC=0;
    int posCellBegin = 0, posFaceBegin=0;
    bitpit::ElementType eltype;
    short PID;
    livector1D conn;
    std::size_t connSize;
    for(const auto & off : offsets){
        idC = bitpit::Cell::NULL_ID;
        PID = 0;
        if(checkCellsID)  {idC= cellsID[counter];}
        if(checkPID)      {PID = pids[counter];}
        eltype = types[counter];
        
        if(eltype == bitpit::ElementType::POLYHEDRON){
            if(!checkFaceOffset){
                throw std::runtime_error("Error VTUGridStreamer : trying to acquire POLYHEDRON info without faces and faceoffsets data");
            }
            connSize = faceoffsets[counter] - posFaceBegin;
            conn.resize(connSize);
            int loc = 0;
            for(int i=posFaceBegin; i<faceoffsets[counter]; ++i){
                conn[loc] = faces[i];
                ++loc;
            }
            //remap vertices: conn is now written face by face with local vertex indices
            int posfbegin = 1, posfend; //begin from 1- value. 0 value of conn contains the total number fo faces
            while(posfbegin < int(connSize)){
                posfend = posfbegin +conn[posfbegin] + 1;
                for(int i=posfbegin+1; i<posfend; ++i){
                    conn[i] = mapVert[conn[i]];
                }
                posfbegin = posfend;
            }
        }else if(eltype == bitpit::ElementType::POLYGON){
            connSize = off - posCellBegin;
            conn.resize(connSize +1);
            conn[0] = connSize;
            int loc =1;
            for(int i=posCellBegin; i<off; ++i){
                conn[loc] = mapVert[connectivitylist[i]];
                ++loc;
            }
        }else{
            connSize = off - posCellBegin;
            conn.resize(connSize);
            int loc =0;
            for(int i=posCellBegin; i<off; ++i){
                conn[loc] = mapVert[connectivitylist[i]];
                ++loc;
            }
        }
        
        bitpit::PatchKernel::CellIterator it = patch.addCell(eltype, true, conn, idC);
        (*it).setPID(PID);
        
        posCellBegin = off;
        if(checkFaceOffset) posFaceBegin = faceoffsets[counter];
        ++counter;
    }
}

/*!
 * Base Constructor
 */
VTUPointCloudStreamer::VTUPointCloudStreamer():VTUAbsorbStreamer(){}

/*!
 * Base Destructor
 */
VTUPointCloudStreamer::~VTUPointCloudStreamer(){}

/*!
 * Absorber of VTU mesh data. Reimplemented from bitpit::VTKBaseStreamer class
 * \param[in] stream    stream to read from
 * \param[in] name      name of the geometry field
 * \param[in] format    ASCII or APPENDED
 * \param[in] entries   number of entries for the data container
 * \param[in] components UNUSED parameter
 */
void VTUPointCloudStreamer::absorbData(std::fstream &stream, std::string name, bitpit::VTKFormat format, 
                                 uint64_t entries, uint8_t components)
{
    BITPIT_UNUSED(components);
    
    if (name == "Points") {
        points.resize(entries);
        for (auto & p :points) {
            if(format == bitpit::VTKFormat::ASCII)  genericIO::absorbASCII(stream,p);
            else                                    genericIO::absorbBINARY(stream, p);
        }
    } else if (name == "vertexIndex") {
        pointsID.resize(entries);
        for (auto & vpid : pointsID) {
            if(format == bitpit::VTKFormat::ASCII)  genericIO::absorbASCII(stream,vpid);
            else                                    genericIO::absorbBINARY(stream, vpid);
        }
    }
}

/*!
 * Use streamer absorbed data, if any, to fill vertices of target bitpit::PatchKernel container, 
 * passed externally. Please notice, container must be empty.
 * \param[in] patch    container for mesh
 */
void VTUPointCloudStreamer::decodeRawData(bitpit::PatchKernel & patch)
{
    //time to check reading result.
    std::size_t nVertices;
    nVertices = points.size();
    if(nVertices == 0){
        throw std::runtime_error("Error VTUPointCloudStreamer : no point coordinates detected while reading *.vtu file.");
    }
    patch.reserveVertices(nVertices);

    //reading mesh nodes and store it in vertices.
    //check labels if any;
    bool checkPointsID;
    {
        std::unordered_set<long> checkSet(pointsID.begin(), pointsID.end());
        checkPointsID = ( checkSet.size() == nVertices);
    }
    //insert points and recover local/global map of vertices;
    std::unordered_map<int, long> mapVert;
    int counter = 0;
    long idV=0;
    for(const auto & p : points){
        idV = bitpit::Vertex::NULL_ID;
        if(checkPointsID) idV = pointsID[counter];
        bitpit::PatchKernel::VertexIterator it = patch.addVertex(p, idV);
        mapVert[counter] = (*it).getId();
        ++counter;
    }
}



/*!
 * Base constructor. Linked reference bitpit::PatchKernel container must be empty. If not, 
 * class will provide to destroy its previous contents and fill it with new read values.
 * \param[in] dir   target directory of file to be read
 * \param[in] name  name fo the file to be read.
 * \param[in] patch reference to empty container for storing mesh data.
 * \param[in] streamer streaming class to absorb VTK data.
 */
VTUGridReader::VTUGridReader( std::string dir, std::string name, VTUAbsorbStreamer & streamer, bitpit::PatchKernel & patch) : 
                              VTKUnstructuredGrid(dir, name, bitpit::VTKElementType::UNDEFINED), m_patch(patch), m_streamer(streamer)
{
    for(auto & field : m_geometry){
        field.enable();
        field.setStreamer(streamer);
    }

    addData<long>("vertexIndex", bitpit::VTKFieldType::SCALAR, bitpit::VTKLocation::POINT, &streamer);
    addData<long>("cellIndex", bitpit::VTKFieldType::SCALAR, bitpit::VTKLocation::CELL, &streamer);
    addData<int>("PID", bitpit::VTKFieldType::SCALAR, bitpit::VTKLocation::CELL, &streamer);
}

/*!
 * Basic Destructor
 */
VTUGridReader::~VTUGridReader(){}

/*!
 * Read the file. Reimplemented from bitpit::VTKUnstructuredGrid::read().
 */
void VTUGridReader::read(){

    readMetaInformation();
    //fields are already checked in readMetaInformation.
    VTKUnstructuredGrid::readData();
     //clear target data
    m_patch.reset();
    m_streamer.decodeRawData(m_patch);
}

/*!
 * Read MetaInformation from file. Reimplemented from bitpit::VTKUnstructuredGrid::readMetaInformation().
 */
void VTUGridReader::readMetaInformation(){
    std::fstream str;
    std::string line, temp;

    std::fstream::pos_type        position;

    str.open( m_fh.getPath( ), std::ios::in ) ;

    getline( str, line);
    while( ! bitpit::utils::string::keywordInString( line, "<VTKFile")){
        getline(str, line);
    }

    if( bitpit::utils::string::getAfterKeyword( line, "header_type", '\"', temp) ){
        VTKUnstructuredGrid::setHeaderType( temp) ;
    }

    while( ! bitpit::utils::string::keywordInString( line, "<Piece")){
        getline(str, line);
    }

    bitpit::utils::string::getAfterKeyword( line, "NumberOfPoints", '\"', temp) ;
    bitpit::utils::string::convertString( temp, m_points );
    
    bitpit::utils::string::getAfterKeyword( line, "NumberOfCells", '\"', temp) ;
    bitpit::utils::string::convertString( temp, m_cells );
    
    
    position = str.tellg() ;
    VTKUnstructuredGrid::readDataHeader( str ) ;
    
    
    for( auto &field : m_geometry ){ 
        str.seekg( position) ;
        if( ! readDataArray( str, field ) ) {
            #if ENABLE_DEBUG
            log::cout() << field.getName() << " DataArray not found" << std::endl ;
            #endif
            field.disable();
        }
    }

    str.close() ;
    setDimensions( m_cells, m_points, readConnectivityEntries(), readFaceStreamEntries() ) ;
}

/*!
 * Reading the total number of entries in the face stream of the vtu file
 * \return total number of faces entries
 */
uint64_t VTUGridReader::readFaceStreamEntries( ){
    uint64_t                 nface(0) ;
    std::fstream             str  ;
    std::fstream::pos_type   position_appended;
    std::string              line;
    char                     c_ ;
    uint32_t                 nbytes32 ;
    uint64_t                 nbytes64 ;


    // Geometry id of the facestream
    int facestream_gid = VTKUnstructuredGrid::_findFieldIndex("faces", m_geometry);
    if(facestream_gid < 0 ) return nface;
    if(!m_geometry[facestream_gid].isEnabled()) return nface;

    if( m_geometry[facestream_gid].getCodification() == VTKFormat::APPENDED ){
        str.open( m_fh.getPath( ), std::ios::in ) ;
        //Go to the initial position of the appended section
        while( getline(str, line) && (! bitpit::utils::string::keywordInString( line, "<AppendedData")) ){}
        str >> c_;
        while( c_ != '_') str >> c_;
        position_appended = str.tellg();
        str.close();
        str.clear();
    }
    
    //Open in binary for read
    if( m_geometry[facestream_gid].getCodification() == VTKFormat::APPENDED ){
        str.open( m_fh.getPath( ), std::ios::in | std::ios::binary);
        str.seekg( position_appended) ;
        str.seekg( m_geometry[facestream_gid].getOffset(), std::ios::cur) ;

        int dataSize = VTKTypes::sizeOfType( m_geometry[facestream_gid].getDataType() ) ;
        assert(dataSize > 0) ;

        if( m_headerType== "UInt32") {
            genericIO::absorbBINARY( str, nbytes32 ) ;
            nface = nbytes32 / dataSize ;
        }

        if( m_headerType== "UInt64") {
            genericIO::absorbBINARY( str, nbytes64 ) ;
            nface = nbytes64 / dataSize ;
        }
        
        str.close();
    }

    //Read geometry ASCII
    if(  m_geometry[facestream_gid].getCodification() == VTKFormat::ASCII ){
        str.open( m_fh.getPath( ), std::ios::in );
        str.seekg( m_geometry[facestream_gid].getPosition() ) ;

        std::string              line ;
        std::vector<uint64_t>    temp;

        nface = 0 ;

        getline( str, line) ;
        while( ! bitpit::utils::string::keywordInString(line,"/DataArray") ) {
            temp.clear() ;
            bitpit::utils::string::convertString( line, temp) ;
            nface += temp.size() ;
            getline( str, line) ;
        }
        
        str.close();
    }

    
    return nface ;
}

/*!
 * Reading the total number of entries in the connectivity stream of the vtu file
 * \return total number of connectivity entries
 */
uint64_t VTUGridReader::readConnectivityEntries( ){
    uint64_t                 nconn(0) ;
    std::fstream             str  ;
    std::fstream::pos_type   position_appended;
    std::string              line;
    char                     c_ ;
    uint32_t                 nbytes32 ;
    uint64_t                 nbytes64 ;
    
    // Geometry id of the connectivity
    int connectivity_gid = VTKUnstructuredGrid::_findFieldIndex("connectivity", m_geometry);
    if(connectivity_gid < 0 ) return nconn;
    
    if( m_geometry[connectivity_gid].getCodification() == VTKFormat::APPENDED ){
        str.open( m_fh.getPath( ), std::ios::in ) ;
        //Go to the initial position of the appended section
        while( getline(str, line) && (! bitpit::utils::string::keywordInString( line, "<AppendedData")) ){}
        str >> c_;
        while( c_ != '_') str >> c_;
        position_appended = str.tellg();
        str.close();
        str.clear();
    }
    
    //Open in binary for read
    if( m_geometry[connectivity_gid].getCodification() == VTKFormat::APPENDED ){
        str.open( m_fh.getPath( ), std::ios::in | std::ios::binary);
        str.seekg( position_appended) ;
        str.seekg( m_geometry[connectivity_gid].getOffset(), std::ios::cur) ;
        
        int dataSize = VTKTypes::sizeOfType( m_geometry[connectivity_gid].getDataType() ) ;
        assert(dataSize > 0) ;
        
        if( m_headerType== "UInt32") {
            genericIO::absorbBINARY( str, nbytes32 ) ;
            nconn = nbytes32 / dataSize ;
        }
        
        if( m_headerType== "UInt64") {
            genericIO::absorbBINARY( str, nbytes64 ) ;
            nconn = nbytes64 / dataSize ;
        }
        str.close();
        str.clear();
    }
    
    //Read geometry ASCII
    if(  m_geometry[connectivity_gid].getCodification() == VTKFormat::ASCII ){
        str.open( m_fh.getPath( ), std::ios::in);
        str.seekg( m_geometry[connectivity_gid].getPosition() ) ;

        std::string              line ;
        std::vector<uint64_t>    temp;
        
        nconn = 0 ;
        
        getline( str, line) ;
        while( ! bitpit::utils::string::keywordInString(line,"/DataArray") ) {
            temp.clear() ;
            bitpit::utils::string::convertString( line, temp) ;
            nconn += temp.size() ;
            getline( str, line) ;
        }
        
        str.close();
        str.clear();
    }
    
    return nconn;
}

}



