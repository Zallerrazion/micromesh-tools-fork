/*
 * SPDX-FileCopyrightText: Copyright (c) 2021, 2022 Universit� degli Studi di Milano. All rights reserved.
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

#include "gui.h"

#include <GLFW/glfw3.h>

#include <imgui.h>

#include <iostream>

void GUIApplication::key_callback(GLFWwindow* window, int key, int, int action, int)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureKeyboard == false) {
		GUIApplication* app = (GUIApplication *) glfwGetWindowUserPointer(window);
		if (app->file_loaded) {
			if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9 && app->quality.current_mode != Quality::None) {
				app->quality.goto_rank = key - GLFW_KEY_1 + 1;
				Assert(app->quality.goto_rank > 0);
				app->set_view_from_quality_rank();
			}
			else if (key == GLFW_KEY_V && action == GLFW_PRESS) {
				app->gui.show_top_mesh = !app->gui.show_top_mesh;
				app->_require_full_draw();
			}
			else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
				app->gui.show_rendering_options = !app->gui.show_rendering_options;
			}
			else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
				app->gui.screenshot.on = true;
			}
			else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
				app->control.update_polar_angles(radians(30) / 0.005, 0);
				app->_require_full_draw();
			}
			else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
				app->control.update_polar_angles(-radians(30) / 0.005, 0);
				app->_require_full_draw();
			}
			else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
				app->control.update_polar_angles(0, radians(30) / 0.005);
				app->_require_full_draw();
			}
			else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
				app->control.update_polar_angles(0, -radians(30) / 0.005);
				app->_require_full_draw();
			}
		}
	}
}

void GUIApplication::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse == false) {
		GUIApplication* app = (GUIApplication *) glfwGetWindowUserPointer(window);
		if (app->file_loaded) {
			app->control.update_polar_radius(yoffset);
			app->_require_full_draw();
		}
	}
}

void GUIApplication::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	int fbwidth, fbheight;
	glfwGetFramebufferSize(window, &fbwidth, &fbheight);
	double scale = fbwidth / (double)width;
	xpos *= scale;
	ypos *= scale;
	
	GUIApplication* app = (GUIApplication *) glfwGetWindowUserPointer(window);

	if (app->file_loaded) {
		Scalar dx = (xpos - app->window.mx) / app->window.width;
		Scalar dy = (ypos - app->window.my) / app->window.height;

		if (app->gui.edit.active) {
			app->_edit_update();
			app->_require_full_draw();
		}
		else {
			if (app->control_drag) {
				app->control.update_polar_angles(xpos - app->window.mx, ypos - app->window.my);
				app->_require_full_draw();
			}
			if (app->control_pan) {
				app->control.pan(dx, dy);
				app->_require_full_draw();
			}
		}
	}

	app->window.mx = xpos;
	app->window.my = ypos;
}

void GUIApplication::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse == false || action == GLFW_RELEASE) {
		GUIApplication* app = (GUIApplication*)glfwGetWindowUserPointer(window);

		if (app->file_loaded) {

			if (action == GLFW_PRESS && !app->gui.edit.active) {
				if (button == GLFW_MOUSE_BUTTON_LEFT && mods == 0) {
					double time_since_last_click = app->click_timer.time_elapsed();
					app->click_timer.reset();

					if (time_since_last_click < 0.25 && !(mods & (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))) {
						app->center_on_mouse_cursor();
						app->_require_full_draw();
					}
					else {
						app->control_drag = true;
					}
				}

				if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
					if (mods & GLFW_MOD_ALT)
						app->split_vertex_near_cursor();
					else if (mods == 0)
						app->control_pan = true;
				}

				//if (button == GLFW_MOUSE_BUTTON_RIGHT && mods == 0) {
				//	app->log_collapse_info_near_cursor();
				//}

				if ((button == GLFW_MOUSE_BUTTON_LEFT) && (mods & GLFW_MOD_ALT)) {
					if (app->ro.current_layer == RenderingOptions::RenderLayer_BaseMesh) {
						app->_edit_init();
					}
				}
				if ((button == GLFW_MOUSE_BUTTON_RIGHT) && (mods & GLFW_MOD_ALT)) {
					if (app->ro.current_layer == RenderingOptions::RenderLayer_MicroMesh || app->ro.current_layer == RenderingOptions::RenderLayer_BaseMesh) {
						app->split_edge_near_cursor();
					}
				}
				if ((button == GLFW_MOUSE_BUTTON_RIGHT) && (mods & GLFW_MOD_SHIFT)) {
					if (app->ro.current_layer == RenderingOptions::RenderLayer_MicroMesh || app->ro.current_layer == RenderingOptions::RenderLayer_BaseMesh) {
						app->flip_edge_near_cursor();
					}
				}
				if ((button == GLFW_MOUSE_BUTTON_RIGHT) && (mods & GLFW_MOD_CONTROL)) {
					if (app->ro.current_layer == RenderingOptions::RenderLayer_MicroMesh) {
						app->select_vertex_near_cursor();
						app->_set_selected_dir_toward_eye();
					}
				}

				if (button == GLFW_MOUSE_BUTTON_LEFT && (mods & GLFW_MOD_SHIFT)) {
					if (app->ro.current_layer == RenderingOptions::RenderLayer_MicroMesh)
						app->tweak_subdivision_level_under_cursor(+1);
				}
				if (button == GLFW_MOUSE_BUTTON_LEFT && (mods & GLFW_MOD_CONTROL)) {
					if (app->ro.current_layer == RenderingOptions::RenderLayer_MicroMesh)
						app->tweak_subdivision_level_under_cursor(-1);
				}
			}

			if (action == GLFW_RELEASE) {
				if (app->gui.edit.active) {
					app->_edit_finalize();
				}
				else {
					if (button == GLFW_MOUSE_BUTTON_LEFT)
						app->control_drag = false;
					if (button == GLFW_MOUSE_BUTTON_MIDDLE)
						app->control_pan = false;
				}
			}
		}
	}
}

void GUIApplication::framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
  printf("Framebuffer size: %d x %d\n", w, h);
	int iconified = glfwGetWindowAttrib(window, GLFW_ICONIFIED);
	if (iconified == GLFW_FALSE) {
		GUIApplication* app = (GUIApplication*)glfwGetWindowUserPointer(window);
		app->window.width = w;
		app->window.height = h;
		app->control.camera.aspect = w / (float)h;
		app->_resize_offscreen_buffers();
	}
}

