cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}")
option(WITH_NVENC "add Nvidia HW encoder support")
option(WITH_DIRECTX "force GDI capture mode" ON)

project( server )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#init 
set(BOOST_ROOT "C:/boost_1_58_0")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package( Boost REQUIRED COMPONENTS thread )

# BOOST

if(Boost_FOUND)
    message("Boost found!")
endif()

if(WITH_NVENC)
	message("add Nvidia HW encoder support")
	find_package(CUDA)
	set(NVENCODER_FOUND 1)
else()
	message("Use software decoder with ffmpeg")
	set(FFMPEG_ROOT "C:/ffmpeg")
endif()

include_directories(${Boost_INCLUDE_DIR})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

# FFMPEG

if (FFMPEG_ROOT)
	FIND_PATH( FFMPEG_INCLUDE_DIR libavcodec/avcodec.h
	  ${FFMPEG_ROOT}/include
	)
	include_directories(${FFMPEG_INCLUDE_DIR})

	FIND_LIBRARY( FFMPEG_LIBRARY_avcodec avcodec
	  ${FFMPEG_ROOT}/lib
	)
	FIND_LIBRARY( FFMPEG_LIBRARY_avutil avutil
	  ${FFMPEG_ROOT}/lib
	)
	if (FFMPEG_LIBRARY_avcodec AND FFMPEG_LIBRARY_avutil)
		set (FFMPEG_FOUND 1)
		set (FFMPEG_LIBRARIES "${FFMPEG_LIBRARY_avcodec};${FFMPEG_LIBRARY_avutil}")
		message("FFMPEG found!")
	endif()
	if (FFMPEG_FOUND)
		SET(FFMPEG_HEADER
		  encoder_ffmpeg/FFMPEG_encoding.hpp
		)
	endif()
endif()

# DXGI and CUDA

if(CUDA_FOUND)
    message("CUDA found!")
	include_directories(${CUDA_TOOLKIT_INCLUDE})
	CUDA_ADD_LIBRARY(cudalib STATIC
    encoder_nvenc/color_conversion.h
    encoder_nvenc/color_conversion.cu
    OPTIONS -arch sm_20
	)
	
	TARGET_LINK_LIBRARIES(cudalib ${CUDA_LIBRARIES})
	set (CUDA_LINK_LIBRARIES "${CUDA_CUDA_LIBRARY};${CUDA_CUDART_LIBRARY};cudalib")
else()
	message("CUDA not found")
endif()



#files for NVEncoder
if (NVENCODER_FOUND) 
	SET(NVENCODER_SOURCE encoder_nvenc/NvEncoder/NvHWEncoder.cpp)
	SET(NVENCODER_HEADERS
	  encoder_nvenc/NV_encoding.hpp
	  encoder_nvenc/NvEncoder/NvEncoder.h
	  encoder_nvenc/NvEncoder/NvHWEncoder.h
	  encoder_nvenc/NvEncoder/nvEncodeAPI.h
	)
endif()		

if(WITH_DIRECTX)
	find_package( DirectX )
else()
	set(DIRECTX_FOUND 0)
endif()
	
include_directories(${DXGI_INCLUDES})
include_directories(${Boost_INCLUDE_DIRS})

if (DIRECTX_FOUND)
	message("set directX capture mode")
	SET(Capture_HEADER
	  windows/wddm.h
	  windows/WDDMCapture.h
	)
else()
	message("set GDI capture mode")
	SET(Capture_HEADER
	  windows/GDICapture.h
	)
endif()

SET(COMMON_SOURCE
  config.h
  bounded_buffer.h
  Capture.h
  fps.h
  monitor.h
  params.h
  windows_controller.hpp
  controller.hpp
)

add_executable( server server.cpp ${COMMON_SOURCE} ${NVENCODER_SOURCE} ${NVENCODER_HEADERS} ${FFMPEG_HEADER} ${Capture_HEADER} )


target_link_libraries( server ${Boost_LIBRARIES} ${FFMPEG_LIBRARIES} ${DXGI_LIBRARIES} ${CUDA_LINK_LIBRARIES})

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)