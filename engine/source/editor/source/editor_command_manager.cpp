#include "editor/include/editor_command_manager.h"
#include "editor/include/editor_scene_manager.h"
#include "editor/include/editor_global_context.h"


namespace Piccolo
{
    //must be called for undo and redo
    void CommandManager::executeCommand(std::shared_ptr<Command> command)
    {
        command->execute();
        undoStack.push_back(std::shared_ptr<Command>(command));
        redoStack.clear(); // Clear the undo stack whenever a new command is executed
    }

    void CommandManager::undo()
    {
        if (!undoStack.empty())
        {
            auto command = std::move(undoStack.back());
            command->undo();
            
            undoStack.pop_back();
            redoStack.push_back(std::move(command));
        }
    }

    void CommandManager::redo()
    {
        if (!redoStack.empty())
        {
            auto command = std::move(redoStack.back());
            command->redo();
            redoStack.pop_back();
            undoStack.push_back(std::move(command));
        }
    }

    /*EditorMoveEntityCommand::EditorMoveEntityCommand(float     new_mouse_pos_x,
                                                     float     new_mouse_pos_y,
                                                     float     last_mouse_pos_x,
                                                     float     last_mouse_pos_y,
                                                     Vector2   engine_window_pos,
                                                     Vector2   engine_window_size,
                                                     size_t    cursor_on_axis,
                                                     Matrix4x4 model_matrix)
    {
        
    }*/

    void EditorMoveEntityCommand::execute() {
       /* g_editor_global_context.m_scene_manager->moveEntity(
            xpos,
            ypos,
            m_mouse_x,
            m_mouse_y,
            m_engine_window_pos,
            m_engine_window_size,
            m_cursor_on_axis,
            g_editor_global_context.m_scene_manager->getSelectedObjectMatrix());*/

    }
    void EditorMoveEntityCommand::start_moving() { 
        std::shared_ptr<GObject> object = g_editor_global_context.m_scene_manager->getSelectedGObject().lock();
        if (object == nullptr)
            return;
        original_matrix = g_editor_global_context.m_scene_manager->getSelectedObjectMatrix();
        
    }

    void EditorMoveEntityCommand::stop_moving() { 
        std::shared_ptr<GObject> object = g_editor_global_context.m_scene_manager->getSelectedGObject().lock();
        if (object == nullptr)
            return;
        destiney_matrix = g_editor_global_context.m_scene_manager->getSelectedObjectMatrix();

    }

    void EditorMoveEntityCommand::undo() { 
        
        g_editor_global_context.m_scene_manager->transformEntity(original_matrix);
        g_editor_global_context.m_scene_manager->setSelectedObjectMatrix(original_matrix);
        g_editor_global_context.m_scene_manager->drawSelectedEntityAxis();

    }
    void EditorMoveEntityCommand::redo() { 
        
        g_editor_global_context.m_scene_manager->transformEntity(destiney_matrix);
        g_editor_global_context.m_scene_manager->setSelectedObjectMatrix(destiney_matrix);

        g_editor_global_context.m_scene_manager->drawSelectedEntityAxis();
    }

    void EditorAddEntityCommand::execute() {

    }
    void EditorAddEntityCommand::undo() {

    }
    
    void EditorDeleteEntityCommand::execute() {

    }
    void EditorDeleteEntityCommand::undo(){

    }
} // namespace Piccolo
