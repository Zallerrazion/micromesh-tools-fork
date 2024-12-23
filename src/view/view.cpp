﻿/*
 * SPDX-FileCopyrightText: Copyright (c) 2021, 2022 Università degli Studi di Milano. All rights reserved.
 *                         Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef _MSC_VER

// Force use of discrete nvidia gpu
#include <windows.h>
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

#endif

#include "gui_view.h"

int main(int argc, char *argv[])
{
	GUIApplication app;

	app.start(argv[1], 
            Vector3f(atof(argv[2]), atof(argv[3]), atof(argv[4])), 
            Vector3f(atof(argv[5]), atof(argv[6]), atof(argv[7])),
            atoi(argv[8]), atoi(argv[9]));

	return 0;
}

