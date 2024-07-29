#include <iostream>
#include <vector>
#include <string>
#include <stack>

using namespace std;

class TaskMemento {
public:
    TaskMemento(const string& desc, bool completed, const string& dueDate)
        : description(desc), isCompleted(completed), dueDate(dueDate) {}

    const string& getDescription() const { return description; }
    bool getCompletedStatus() const { return isCompleted; }
    const string& getDueDate() const { return dueDate; }

private:
    string description;
    bool isCompleted;
    string dueDate;
};

class Task {
public:
    class Builder {
    public:
        Builder(const string& desc) : description(desc), completed(false) {}

        Builder& setDueDate(const string& date) {
            dueDate = date;
            return *this;
        }

        Builder& setTags(const vector<string>& taskTags) {
            tags = taskTags;
            return *this;
        }

        Task build() const {
            return Task(description, completed, dueDate, tags);
        }

    private:
        string description;
        bool completed;
        string dueDate;
        vector<string> tags;

        friend class Task;
    };

    void markCompleted() {
        if (!completed) {
            completed = true;
        }
    }

    void markPending() {
        if (completed) {
            completed = false;
        }
    }

    bool isCompleted() const {
        return completed;
    }

    const string& getDescription() const {
        return description;
    }

    void display(int index) const {
        cout << index + 1 << ". " << description << " - " << (completed ? "Completed" : "Pending");
        if (!dueDate.empty()) {
            cout << ", Due: " << dueDate;
        }
        cout << endl;
    }

    TaskMemento save() const {
        return TaskMemento(description, completed, dueDate);
    }

    void restore(const TaskMemento& memento) {
        description = memento.getDescription();
        completed = memento.getCompletedStatus();
        dueDate = memento.getDueDate();
    }

private:
    Task(const string& desc, bool isCompleted, const string& date, const vector<string>& taskTags)
        : description(desc), completed(isCompleted), dueDate(date), tags(taskTags) {}

    string description;
    bool completed;
    string dueDate;
    vector<string> tags;
};

class TaskHistory {
public:
    void addMemento(const TaskMemento& memento) {
        history.push(memento);
        redoStack = stack<TaskMemento>(); // Clear redo stack when a new action is performed
    }

    TaskMemento getMemento() {
        TaskMemento memento = history.top();
        history.pop();
        redoStack.push(memento);
        return memento;
    }

    bool isEmpty() const {
        return history.empty();
    }

    bool isRedoStackEmpty() const {
        return redoStack.empty();
    }

    TaskMemento redo() {
        TaskMemento memento = redoStack.top();
        redoStack.pop();
        history.push(memento);
        return memento;
    }

private:
    stack<TaskMemento> history;
    stack<TaskMemento> redoStack;
};

class ToDoListManager {
public:
    void addTask(const Task& task) {
        tasks.push_back(task);
        history.addMemento(task.save());
    }

    void markTaskCompleted(int index) {
        if (index >= 0 && index < tasks.size() && !tasks[index].isCompleted()) {
            history.addMemento(tasks[index].save());
            tasks[index].markCompleted();
        }
    }

    void markTaskPending(int index) {
        if (index >= 0 && index < tasks.size() && tasks[index].isCompleted()) {
            history.addMemento(tasks[index].save());
            tasks[index].markPending();
        }
    }

    void deleteTask(int index) {
        if (index >= 0 && index < tasks.size()) {
            history.addMemento(tasks[index].save());
            tasks.erase(tasks.begin() + index);
        }
    }

    void viewTasks(const string& filter) const {
        cout << "Tasks:" << endl;

        for (size_t i = 0; i < tasks.size(); ++i) {
            if (filter == "Show all" ||
                (filter == "Show completed" && tasks[i].isCompleted()) ||
                (filter == "Show pending" && !tasks[i].isCompleted())) {
                tasks[i].display(i);
            }
        }
    }

    void undo() {
        if (!history.isEmpty()) {
            TaskMemento memento = history.getMemento();
            redoStack.addMemento(memento); // Store in redo stack before restoring
            for (auto& task : tasks) {
                if (task.getDescription() == memento.getDescription()) {
                    task.restore(memento);
                    break;
                }
            }
            cout << "Undo successful." << endl;
        } else {
            cout << "Nothing to undo." << endl;
        }
    }

    void redo() {
        if (!redoStack.isRedoStackEmpty()) {
            TaskMemento memento = redoStack.redo();
            history.addMemento(memento);
            for (auto& task : tasks) {
                if (task.getDescription() == memento.getDescription()) {
                    task.restore(memento);
                    break;
                }
            }
            cout << "Redo successful." << endl;
        } else {
            cout << "Nothing to redo." << endl;
        }
    }

private:
    vector<Task> tasks;
    TaskHistory history;
    TaskHistory redoStack;
};

int main() {
    ToDoListManager manager;

    while (true) {
        cout << "What would you like to do?" << endl;
        cout << "1. Add a new task" << endl;
        cout << "2. Mark a task as completed" << endl;
        cout << "3. Mark a task as pending" << endl;
        cout << "4. Delete a task" << endl;
        cout << "5. View all tasks" << endl;
        cout << "6. View completed tasks" << endl;
        cout << "7. View pending tasks" << endl;
        cout << "8. Undo" << endl;
        cout << "9. Redo" << endl;
        cout << "10. Exit" << endl;

        int choice;
        cin >> choice;

        switch (choice) {
            case 1: {
                string description, due_date;
                cout << "Enter task description: ";
                cin.ignore();
                getline(cin, description);

                string addDueDate;
                cout << "Do you want to add a due date? (y/n): ";
                cin >> addDueDate;

                if (addDueDate == "y" || addDueDate == "Y") {
                    cout << "Enter due date (YYYY-MM-DD): ";
                    cin >> due_date;
                }

                Task task = Task::Builder(description).setDueDate(due_date).build();
                manager.addTask(task);
                cout << "Task added successfully!" << endl;
                break;
            }
            case 2: {
                int index;
                cout << "Enter task index: ";
                cin >> index;
                manager.markTaskCompleted(index - 1);
                cout << "Task marked as completed!" << endl;
                break;
            }
            case 3: {
                int index;
                cout << "Enter task index: ";
                cin >> index;
                manager.markTaskPending(index - 1);
                cout << "Task marked as pending!" << endl;
                break;
            }
            case 4: {
                int index;
                cout << "Enter task index: ";
                cin >> index;
                manager.deleteTask(index - 1);
                cout << "Task deleted successfully!" << endl;
                break;
            }
            case 5: {
                manager.viewTasks("Show all");
                break;
            }
            case 6: {
                manager.viewTasks("Show completed");
                break;
            }
            case 7: {
                manager.viewTasks("Show pending");
                break;
            }
            case 8: {
                manager.undo();
                break;
            }
            case 9: {
                manager.redo();
                break;
            }
            case 10: {
                cout << "Exiting..." << endl;
                return 0;
            }
            default: {
                cout << "Invalid choice. Please try again." << endl;
                break;
            }
        }
    }
}
