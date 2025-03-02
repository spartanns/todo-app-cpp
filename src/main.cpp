#include "imgui.h"
#include "../backends/imgui_impl_glfw.h"
#include "../backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct TodoItem {
    std::string text;
    bool completed;

    TodoItem(const std::string& task) : text(task), completed(false) {}
};

class TodoApp {
private:
    std::vector<TodoItem> todos;
    char inputBuffer[256];
    bool showCompleted;

public:
    TodoApp() : inputBuffer{0}, showCompleted(true) {}

    void render() {
        ImGui::Begin("Todo App");

        ImGui::Text("Add a new task:");
        if (ImGui::InputText("##input", inputBuffer, sizeof(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (inputBuffer[0] != '\0') {
                todos.emplace_back(inputBuffer);
                inputBuffer[0] = '\0';
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Add") && inputBuffer[0] != '\0') {
            todos.emplace_back(inputBuffer);
            inputBuffer[0] = '\0';
        }

        ImGui::Separator();

        ImGui::Checkbox("Show Completed Tasks", &showCompleted);

        ImGui::Separator();

        if (ImGui::BeginTable("todos", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Status");
            ImGui::TableSetupColumn("Task");
            ImGui::TableSetupColumn("Actions");
            ImGui::TableHeadersRow();

            for (size_t i = 0; i < todos.size(); i++) {
                if (!showCompleted && todos[i].completed)
                    continue;

                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                std::string checkbox_id = "##checkbox" + std::to_string(i);
                if (ImGui::Checkbox(checkbox_id.c_str(), &todos[i].completed)) {
                }

                ImGui::TableNextColumn();
                if (todos[i].completed) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                    ImGui::Text("%s", todos[i].text.c_str());
                    ImGui::PopStyleColor();
                } else {
                    ImGui::Text("%s", todos[i].text.c_str());
                }

                ImGui::TableNextColumn();
                std::string delete_button_id = "Delete##" + std::to_string(i);
                if (ImGui::Button(delete_button_id.c_str())) {
                    todos.erase(todos.begin() + i);
                    i--;
                }
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }
};

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Todo List App", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    /*
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }
    */

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    io.Fonts->AddFontDefault();

    TodoApp app;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        app.render();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
