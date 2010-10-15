// Copyright 2006, Willem van Engen
//
// AIM-C: A C++ implementation of the Auditory Image Model
// http://www.acousticscale.org/AIMC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Support/Common.h"
#include "Modules/Output/Graphics/Devices/GraphicsOutputDevice.h"

GraphicsOutputDevice::GraphicsOutputDevice(Parameters *pParam) {
	m_pParam = pParam;
}

void GraphicsOutputDevice::gVertex3f(float x,
                                     float y,
                                     float z,
                                     float r,
                                     float g,
                                     float b) {
	gColor3f(r, g, b);
	gVertex3f(x, y, z);
}

void GraphicsOutputDevice::gVertex2f(float x,
                                     float y,
                                     float r,
                                     float g,
                                     float b) {
	gColor3f(r, g, b);
	gVertex3f(x, y, 0);
}

void GraphicsOutputDevice::gVertex2f(float x, float y) {
	gVertex3f(x, y, 0);
}

void GraphicsOutputDevice::gText2f(float x,
                                   float y,
                                   const char *sStr,
                                   bool bRotated) {
	gText3f(x, y, 0, sStr, bRotated);
}
