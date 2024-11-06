#pragma once
#include<vector>
#include"editor/include/editor_scene_manager.h"

namespace Piccolo
{
    class Command
    {
    public:
        virtual ~Command() {};
        virtual void execute() = 0;
        virtual void undo()    = 0;
        virtual void redo() { 
            execute();
        };
    };

    class EditorMoveEntityCommand: public Command
    {
    public:
        void execute();
        void undo();
        void start_moving();
        void stop_moving();
        void redo() override;

    private:
        Matrix4x4 original_matrix;
        Matrix4x4 destiney_matrix;

    };

    class EditorAddEntityCommand :public Command
    {
    public:
        void execute();
        
        void undo();
        void redo();
    private:
    };

    class EditorDeleteEntityCommand : public Command
    {
    public:
        void execute();
        void undo();

    private:
    };

    

    class CommandManager
    {
    private:
        std::vector<std::shared_ptr<Command>> undoStack;
        std::vector<std::shared_ptr<Command>> redoStack;

    public:
        //Command excute function will be called
        void executeCommand(std::shared_ptr<Command> command);
        void undo();
        void redo();
    };


    extern CommandManager command_manager;

}

