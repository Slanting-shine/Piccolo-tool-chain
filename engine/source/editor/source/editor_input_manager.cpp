#include "editor/include/editor_input_manager.h"

#include "editor/include/editor.h"
#include "editor/include/editor_global_context.h"
#include "editor/include/editor_scene_manager.h"
#include "editor/include/editor_command_manager.h"

#include "runtime/engine.h"
#include "runtime/function/framework/level/level.h"
#include "runtime/function/framework/world/world_manager.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/input/input_system.h"

#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_system.h"
#include "runtime/function/render/window_system.h"
#include <core/base/macro.h>

namespace Piccolo
{
    void EditorInputManager::initialize() { registerInput(); }

    void EditorInputManager::tick(float delta_time) { processEditorCommand(); }

    void EditorInputManager::registerInput()
    {
        g_editor_global_context.m_window_system->registerOnResetFunc(std::bind(&EditorInputManager::onReset, this));
        g_editor_global_context.m_window_system->registerOnCursorPosFunc(
            std::bind(&EditorInputManager::onCursorPos, this, std::placeholders::_1, std::placeholders::_2));
        g_editor_global_context.m_window_system->registerOnCursorEnterFunc(
            std::bind(&EditorInputManager::onCursorEnter, this, std::placeholders::_1));
        g_editor_global_context.m_window_system->registerOnScrollFunc(
            std::bind(&EditorInputManager::onScroll, this, std::placeholders::_1, std::placeholders::_2));
        g_editor_global_context.m_window_system->registerOnMouseButtonFunc(
            std::bind(&EditorInputManager::onMouseButtonClicked, this, std::placeholders::_1, std::placeholders::_2));
        g_editor_global_context.m_window_system->registerOnWindowCloseFunc(
            std::bind(&EditorInputManager::onWindowClosed, this));
        g_editor_global_context.m_window_system->registerOnKeyFunc(std::bind(&EditorInputManager::onKey,
                                                                             this,
                                                                             std::placeholders::_1,
                                                                             std::placeholders::_2,
                                                                             std::placeholders::_3,
                                                                             std::placeholders::_4));
    }

    void EditorInputManager::updateCursorOnAxis(Vector2 cursor_uv)
    {
        if (g_editor_global_context.m_scene_manager->getEditorCamera())
        {
            Vector2 window_size(m_engine_window_size.x, m_engine_window_size.y);
            m_cursor_on_axis = g_editor_global_context.m_scene_manager->updateCursorOnAxis(cursor_uv, window_size);
        }
    }

    void EditorInputManager::processEditorCommand()
    {
        float           camera_speed  = m_camera_speed;
        std::shared_ptr editor_camera = g_editor_global_context.m_scene_manager->getEditorCamera();
        Quaternion      camera_rotate = editor_camera->rotation().inverse();
        Vector3         camera_relative_pos(0, 0, 0);

        if ((unsigned int)EditorInputCommand::camera_foward & m_editor_command)
        {
            camera_relative_pos += camera_rotate * Vector3 {0, camera_speed, 0};
        }
        if ((unsigned int)EditorInputCommand::camera_back & m_editor_command)
        {
            camera_relative_pos += camera_rotate * Vector3 {0, -camera_speed, 0};
        }
        if ((unsigned int)EditorInputCommand::camera_left & m_editor_command)
        {
            camera_relative_pos += camera_rotate * Vector3 {-camera_speed, 0, 0};
        }
        if ((unsigned int)EditorInputCommand::camera_right & m_editor_command)
        {
            camera_relative_pos += camera_rotate * Vector3 {camera_speed, 0, 0};
        }
        if ((unsigned int)EditorInputCommand::camera_up & m_editor_command)
        {
            camera_relative_pos += Vector3 {0, 0, camera_speed};
        }
        if ((unsigned int)EditorInputCommand::camera_down & m_editor_command)
        {
            camera_relative_pos += Vector3 {0, 0, -camera_speed};
        }
        if ((unsigned int)EditorInputCommand::delete_object & m_editor_command)
        {
            g_editor_global_context.m_scene_manager->onDeleteSelectedGObject();
        }
  
        editor_camera->move(camera_relative_pos);
    }

    void EditorInputManager::onKeyInEditorMode(int key, int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            if (mods & GLFW_MOD_CONTROL)
            { // 检查是否按下了控制键
                switch (key)
                {
                    case GLFW_KEY_Z:
                        //if (!undo_pressed)
                        //{
                        //    // 执行撤销操作
                        //    m_editor_command |= static_cast<unsigned int>(EditorInputCommand::undo);
                        //    undo_pressed = true; // 设置撤销键已按下的标志位
                        //}
                        g_editor_global_context.m_command_manager->undo();
                        LOG_INFO("undo")
                        break;
                    case GLFW_KEY_Y:
                        //if (!redo_pressed)
                        //{
                        //    // 执行重做操作
                        //    m_editor_command |= static_cast<unsigned int>(EditorInputCommand::redo);
                        //    redo_pressed = true; // 设置重做键已按下的标志位
                        //}
                        //break;
                        g_editor_global_context.m_command_manager->redo();
                        LOG_INFO("redo")
                }
            }

            switch (key)
            {
                case GLFW_KEY_A:
                    m_editor_command |= (unsigned int)EditorInputCommand::camera_left;
                    break;
                case GLFW_KEY_S:
                    m_editor_command |= (unsigned int)EditorInputCommand::camera_back;
                    break;
                case GLFW_KEY_W:
                    m_editor_command |= (unsigned int)EditorInputCommand::camera_foward;
                    break;
                case GLFW_KEY_D:
                    m_editor_command |= (unsigned int)EditorInputCommand::camera_right;
                    break;
                case GLFW_KEY_Q:
                    m_editor_command |= (unsigned int)EditorInputCommand::camera_up;
                    break;
                case GLFW_KEY_E:
                    m_editor_command |= (unsigned int)EditorInputCommand::camera_down;
                    break;
                case GLFW_KEY_T:
                    m_editor_command |= (unsigned int)EditorInputCommand::translation_mode;
                    break;
                case GLFW_KEY_R:
                    m_editor_command |= (unsigned int)EditorInputCommand::rotation_mode;
                    break;
                case GLFW_KEY_C:
                    m_editor_command |= (unsigned int)EditorInputCommand::scale_mode;
                    break;
                case GLFW_KEY_DELETE:
                    m_editor_command |= (unsigned int)EditorInputCommand::delete_object;
                    break;
                default:
                    break;
            }
        }
        else if (action == GLFW_RELEASE)
        {
            
            if (mods & GLFW_MOD_CONTROL)
            {
                switch (key)
                {
                    //case GLFW_KEY_Z:
                    //    undo_pressed = false; // 重置撤销键的标志位
                    //    break;
                    //case GLFW_KEY_Y:
                    //    redo_pressed = false; // 重置重做键的标志位
                    //    break;
                }
            }

            switch (key)
            {
                case GLFW_KEY_ESCAPE:
                    m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorInputCommand::exit);
                    break;
                case GLFW_KEY_A:
                    m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorInputCommand::camera_left);
                    break;
                case GLFW_KEY_S:
                    m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorInputCommand::camera_back);
                    break;
                case GLFW_KEY_W:
                    m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorInputCommand::camera_foward);
                    break;
                case GLFW_KEY_D:
                    m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorInputCommand::camera_right);
                    break;
                case GLFW_KEY_Q:
                    m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorInputCommand::camera_up);
                    break;
                case GLFW_KEY_E:
                    m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorInputCommand::camera_down);
                    break;
                case GLFW_KEY_T:
                    m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorInputCommand::translation_mode);
                    break;
                case GLFW_KEY_R:
                    m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorInputCommand::rotation_mode);
                    break;
                case GLFW_KEY_C:
                    m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorInputCommand::scale_mode);
                    break;
                case GLFW_KEY_DELETE:
                    m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorInputCommand::delete_object);
                    break;
                default:
                    break;
            }
        }
    }

    void EditorInputManager::onKey(int key, int scancode, int action, int mods)
    {
        if (g_is_editor_mode)
        {
            onKeyInEditorMode(key, scancode, action, mods);
        }
    }

    void EditorInputManager::onReset()
    {
        // to do
    }

    void EditorInputManager::onCursorPos(double xpos, double ypos)
    {
        if (!g_is_editor_mode)
            return;
        
        float angularVelocity =
            180.0f / Math::max(m_engine_window_size.x, m_engine_window_size.y); // 180 degrees while moving full screen
        if (m_mouse_x >= 0.0f && m_mouse_y >= 0.0f)
        {
            if (g_editor_global_context.m_window_system->isMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
            {
                glfwSetInputMode(
                    g_editor_global_context.m_window_system->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                g_editor_global_context.m_scene_manager->getEditorCamera()->rotate(
                    Vector2(ypos - m_mouse_y, xpos - m_mouse_x) * angularVelocity);
            }
            else if (g_editor_global_context.m_window_system->isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
            {
                //LOG_INFO("down\n");
                /*g_editor_global_context.m_scene_manager->moveEntity(
                    xpos,
                    ypos,
                    m_mouse_x,
                    m_mouse_y,
                    m_engine_window_pos,
                    m_engine_window_size,
                    m_cursor_on_axis,
                    g_editor_global_context.m_scene_manager->getSelectedObjectMatrix());*/


                if (g_editor_global_context.m_scene_manager->getSelectedObjectID() != k_invalid_gobject_id)
                {
                    Matrix4x4 new_matrix = g_editor_global_context.m_scene_manager->calculateMoveFromPos(
                        xpos,
                        ypos,
                        m_mouse_x,
                        m_mouse_y,
                        m_engine_window_pos,
                        m_engine_window_size,
                        m_cursor_on_axis,
                        g_editor_global_context.m_scene_manager->getSelectedObjectMatrix());
                    g_editor_global_context.m_scene_manager->transformEntity(new_matrix);
                    is_dragging = true;
                    if (moving_start == false)
                    {
                        LOG_INFO("1")
                        moving_start = true;
                        moving_command = std::make_shared<EditorMoveEntityCommand>();
                        moving_command->start_moving();
                    }
                }

                glfwSetInputMode(g_editor_global_context.m_window_system->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }       
            else
            {
                glfwSetInputMode(g_editor_global_context.m_window_system->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

                if (isCursorInRect(m_engine_window_pos, m_engine_window_size))
                {
                    Vector2 cursor_uv = Vector2((m_mouse_x - m_engine_window_pos.x) / m_engine_window_size.x,
                                                (m_mouse_y - m_engine_window_pos.y) / m_engine_window_size.y);
                    updateCursorOnAxis(cursor_uv);

                }
            }
        }
        m_mouse_x = xpos;
        m_mouse_y = ypos;
    }

    void EditorInputManager::onCursorEnter(int entered)
    {
        if (!entered) // lost focus
        {
            m_mouse_x = m_mouse_y = -1.0f;
        }
    }

    void EditorInputManager::onScroll(double xoffset, double yoffset)
    {
        if (!g_is_editor_mode)
        {
            return;
        }

        if (isCursorInRect(m_engine_window_pos, m_engine_window_size))
        {
            if (g_editor_global_context.m_window_system->isMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
            {
                if (yoffset > 0)
                {
                    m_camera_speed *= 1.2f;
                }
                else
                {
                    m_camera_speed *= 0.8f;
                }
            }
            else
            {
                g_editor_global_context.m_scene_manager->getEditorCamera()->zoom(
                    (float)yoffset * 2.0f); // wheel scrolled up = zoom in by 2 extra degrees
            }
        }
    }

    void EditorInputManager::onMouseButtonClicked(int key, int action)
    {
        if (!g_is_editor_mode)
            return;
        /*if (m_cursor_on_axis != 3)
            return;*/

        std::shared_ptr<Level> current_active_level = g_runtime_global_context.m_world_manager->getCurrentActiveLevel().lock();
        if (current_active_level == nullptr)
            return;
        LOG_INFO("mouse buttom clicked")
        if (isCursorInRect(m_engine_window_pos, m_engine_window_size))
        {
            
            if (key == GLFW_MOUSE_BUTTON_LEFT &&
                g_editor_global_context.m_window_system->isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
            {
               
                Vector2 picked_uv((m_mouse_x - m_engine_window_pos.x) / m_engine_window_size.x,
                                  (m_mouse_y - m_engine_window_pos.y) / m_engine_window_size.y);
                size_t  select_mesh_id = g_editor_global_context.m_scene_manager->getGuidOfPickedMesh(picked_uv);

                selected_gobject_id = g_editor_global_context.m_render_system->getGObjectIDByMeshID(select_mesh_id);
                
               /* */

                
            }
            if (key == GLFW_MOUSE_BUTTON_LEFT)
            {
                LOG_INFO("clicked")
            }

            if (key == GLFW_MOUSE_BUTTON_LEFT &&
                g_editor_global_context.m_window_system->isMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT))
            {
                if (is_dragging == false)
                {
                    g_editor_global_context.m_scene_manager->onGObjectSelected(selected_gobject_id);
                }
                LOG_INFO("left buttom up")
                if (selected_gobject_id != k_invalid_gobject_id)
                {
                    LOG_INFO("?")
                    is_dragging = false;
                    if (moving_start == true)
                    {
                        LOG_INFO("2")
                        moving_command->stop_moving();
                        g_editor_global_context.m_command_manager->executeCommand(moving_command);
                        moving_start = false;
                    }
                    

                }
                
            }
        }
    }

    void EditorInputManager::onWindowClosed() { g_editor_global_context.m_engine_runtime->shutdownEngine(); }

    bool EditorInputManager::isCursorInRect(Vector2 pos, Vector2 size) const
    {
        return pos.x <= m_mouse_x && m_mouse_x <= pos.x + size.x && pos.y <= m_mouse_y && m_mouse_y <= pos.y + size.y;
    }
} // namespace Piccolo