#ifndef APP_CPP
#define APP_CPP

#include "app.h"

using namespace glm;

// Callback functions
static void window_size_callback(GLFWwindow* window, int width, int height)
{
  App* app = static_cast<App *>(glfwGetWindowUserPointer(window));
  if (app) {
    app->window_resize_event(width, height);
  } else {
    Err_Print("Cannot get the app ptr during callback", "app.cpp");
  }
}
void App::window_resize_event(int width, int height)
{
  glViewport(0, 0, width, height);
  // Calculate aspect ratio
  float aspect = float(width) / float(height ? height : 1);

  // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
  const float zNear = 1.0, zFar = 1500.0, fov = 45.0;

  // Reset projection
  projection = mat4(1.0f);

  // Set perspective projection
  projection = perspective(radians(fov), aspect, zNear, zFar);

  Info_Print(std::to_string(width)+"x"+std::to_string(height));
}

static void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
  App* app = static_cast<App *>(glfwGetWindowUserPointer(window));
  if (app) {
    app->mouse_cursor_event(xpos, ypos);
  } else {
    Err_Print("Cannot get the app ptr during callback", "app.cpp");
  }
}
void App::mouse_cursor_event(double xpos, double ypos)
{
  if (camera_is_moving)
  {
    vec2 mousepos = vec2(float(xpos), float(-ypos));

    if (firstMouse)
    {
      lastmousepos = mousepos+vec2(0.0001f, 0.00001f);
      firstMouse = false  ;
    }                     
    diff = mousepos-lastmousepos;
    lastmousepos = mousepos;
    if (rot)
    {
    rotationAxis = transpose(rotation) * vec4(-diff[1], diff[0], 0.0f, 1.0f);
    rotationAxis = normalize(rotationAxis);
    rotationAngle = length(diff) * sensitivity;
    rotation = rotate(rotation, radians(rotationAngle), rotationAxis);
    if (rotationAxis[0]!=rotationAxis[0]) {
      Warn_Print("NAN in rotationAxis vector computation for camera view");
      rotationAxis = glm::vec3(0.0f,0.0f,1.0f);
    }
    }
    if (mov)
    {
      camerapos = camerapos + cameradist * 0.01f * sensitivity * vec3(transpose(rotation) * vec4(diff[0], diff[1], 0.0f, 1.0f));
    }
    if (scale)
    {
      Info_Print(std::to_string(length(diff)));
      // Rajouter une var qui dit si on se rapproche de l'écran ou non
      cameradist = cameradist + length(diff);
    }
  }
}


static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  App* app = static_cast<App *>(glfwGetWindowUserPointer(window));
  if (app) {
    app->mouse_button_event(button, action, mods);
  } else {
    Err_Print("Cannot get the app ptr during callback", "app.cpp");
  }
}
void App::mouse_button_event(int button, int action, int mods)
{
  if (button==GLFW_MOUSE_BUTTON_LEFT && action==GLFW_PRESS)
  {
    Info_Print("Mouse button pressed");
    rot = true;
    camera_is_moving = true;
  }
  if (button==GLFW_MOUSE_BUTTON_LEFT && action==GLFW_RELEASE)
  {
    rot = false;
    camera_is_moving = false;
    firstMouse = true;
  }
}

void App::processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS)
    mov=true;

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
    mov=false;

  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    scale=true;

  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
    scale=false;
}

// Constructor
App::App() :
  size_window(1.7f), show_demo_window(true), show_another_window(false)
{
  /******** Init OpenGL ********/
  Title_Print("Launch Simuscle App");

  // Init GLFW
  Info_Print("Init GLFW window");
  glfwInit();
  // Choose GLSL version 
  Info_Print("Choose glsl version");
  const char* glsl_version = "#version 420";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  // Create window with graphics context
  Info_Print("Create window");
  window = glfwCreateWindow(size_window*1280, size_window*720, "Simuscle", nullptr, nullptr);
  if (window == nullptr)
    Err_Print("Failed to launch GLFW window", "main.cpp");
    // return 1; //TODO
  glfwMakeContextCurrent(window);
  // Link callback function
  Info_Print("Set callback function");
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, window_size_callback);
  glfwSetCursorPosCallback(window, mouse_cursor_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  glfwSwapInterval(1); // Enable vsync
                       // Load extension's functions
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

  /******** Init Renderer ********/
  // vertex shader
  Title_Print("Vertex shader");

  Info_Print("read vertex shader");
  std::string vshader_src = "base_shader.vert";
  std::ifstream vertexShaderFile("base_shader.vert");
  std::ostringstream vertexBuffer;
  vertexBuffer << vertexShaderFile.rdbuf();
  std::string vertexBufferStr = vertexBuffer.str();
  // Warning: safe only until vertexBufferStr is destroyed or modified
  const GLchar *source_vert = vertexBufferStr.c_str();
  // const GLchar *source_vert = (const GLchar *)vshader_src.c_str();

  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  Info_Print("Vertex shader: ");
  Info_Print(source_vert);
  glShaderSource(vertexShader, 1, &source_vert, 0);
  glCompileShader(vertexShader);
  Info_Print("Done Compile vert Shader");
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      Err_Print(infoLog, source_vert);
      // return 1; //TODO
  }

  // fragment shader
  Title_Print("Fragment shader");

  std::string fshader_src = "base_shader.frag";
  std::ifstream fragShaderFile(fshader_src);
  std::ostringstream fragBuffer;
  fragBuffer << fragShaderFile.rdbuf();
  std::string fragBufferStr = fragBuffer.str();
  // Warning: safe only until vertexBufferStr is destroyed or modified
  const GLchar *source_frag = fragBufferStr.c_str();
  // const GLchar *source_vert = (const GLchar *)vshader_src.c_str();
  //
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  Info_Print(source_frag);
  glShaderSource(fragmentShader, 1, &source_frag, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      Err_Print(infoLog, source_frag);
      // return 1; //TODO
  }
  // link shader
  Info_Print("Link shader");
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
      // return 1; //TODO
  }
  vertexShaderFile.close();
  fragShaderFile.close();
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  /******** Init ImGUI ********/
  Title_Print("Init ImGUI");

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  Info_Print("IO");
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
                                                            //io.ConfigViewportsNoAutoMerge = true;
                                                            //io.ConfigViewportsNoTaskBarIcon = true;

                                                            // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.

  Info_Print("Style");
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  Info_Print("Link Imgui with GLFW");
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  /******* App Var initialisation ********/
  Title_Print("Other var Init");
  // Our state
  // bool show_demo_window = true;
  // bool show_another_window = false;
  // bool up = true;
  // int frame = 0;
  // ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  // view = glm::translate(view, cameraMov);

  frame = 0;
  sensitivity = 0.1f;
  firstMouse=true;
  projection = mat4(1.0f);
  view = mat4(1.0f);
  camerapos = vec3(0.0f, 0.0f, 0.0f);
  cameradist = 5.0f;
  rotation= mat4(1.0f);
  camera_is_moving = false;
  rot = false; mov = false; scale = false;
  // rotationAxis = vec3(1.0f, 0.0f, 0.0f);
  // rotationAngle = 45;
  // rotation = rotate(rotation, radians(-rotationAngle), rotationAxis);



  // clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  // view = glm::translate(view, cameraMov);

  // To get the 1st render strange...
  window_resize_event(size_window*1280, size_window*720);
  Info_Print("End of init");
}


void App::Run()
{
  Title_Print("Run");
  Info_Print("Running");
  std::cout << glfwWindowShouldClose(window) << std::endl;
  std::cout << window << std::endl;
  while (!glfwWindowShouldClose(window))
  {
   // Input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    processInput(window);
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Viewports
    bool b=true;
    bool *p_open;
    p_open = &b;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport", p_open, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    dockspace_flags = ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoResize;

    if (ImGui::BeginMenuBar())
    {
      if (ImGui::BeginMenu("Options"))
      {
        // Disabling fullscreen would allow the window to be moved to the front of other windows,
        // which we can't undo at the moment without finer window depth/z control.
        ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
        ImGui::MenuItem("Padding", NULL, &opt_padding);
        ImGui::Separator();

        if (ImGui::MenuItem("Flag: NoSplit",                "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
        if (ImGui::MenuItem("Flag: NoResize",               "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
        if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
        if (ImGui::MenuItem("Flag: AutoHideTabBar",         "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
        if (ImGui::MenuItem("Flag: PassthruCentralNode",    "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
        ImGui::Separator();

        if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
          *p_open = false;
        ImGui::EndMenu();
      }
      const char* desc =
          "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!" "\n"
          "- Drag from window title bar or their tab to dock/undock." "\n"
          "- Drag from window menu button (upper-left button) to undock an entire node (all windows)." "\n"
          "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)" "\n"
          "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)" "\n"
          "This demo app has nothing to do with enabling docking!" "\n\n"
          "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window." "\n\n"
          "Read comments in ShowExampleAppDockSpace() for more details.";
      ImGui::TextDisabled("(?)");
      if (ImGui::BeginItemTooltip())
      {
          ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
          ImGui::TextUnformatted(desc);
          ImGui::PopTextWrapPos();
          ImGui::EndTooltip();
      }
      // HelpMarker(
      //     "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!" "\n"
      //     "- Drag from window title bar or their tab to dock/undock." "\n"
      //     "- Drag from window menu button (upper-left button) to undock an entire node (all windows)." "\n"
      //     "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)" "\n"
      //     "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)" "\n"
      //     "This demo app has nothing to do with enabling docking!" "\n\n"
      //     "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window." "\n\n"
      //     "Read comments in ShowExampleAppDockSpace() for more details.");

      ImGui::EndMenuBar();

    }

    ImGui::End();


    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    ImGui::ShowDemoWindow();

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    // Rendering
    ImGui::Render();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Camera transform
    // glm::mat4 view = glm::mat4(1.00f);
    // view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));
    // view  = glm::rotate(view, float(frame)/1000*6.28f, glm::vec3(0.0f, 0.0f, 1.0f));
    // Mat4_Print("Translated view matrix: ", view);
    //view  = glm::rotate(view, glm::radians(rotationAngle), rotationAxis);
    //Info_Print(std::to_string(rotationAngle));
    //Info_Print(std::to_string(rotationAxis[0]));
    //Info_Print(std::to_string(rotationAxis[1]));
    //Info_Print(std::to_string(rotationAxis[2]));
    //Mat4_Print("Translated and Rotated view matrix: ", view);
    view = mat4(1.0f);
    view = glm::translate(view, vec3(0.0f, 0.0f, -cameradist));
    view = view * rotation;
    view = translate(view, camerapos);
    glm::mat4 vp_mat = projection * view;

    int modelLoc = glGetUniformLocation(shaderProgram, "vp_mat");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(vp_mat));

    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



    // glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    // glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    //
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(window);
  }

}



App::~App()
{
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
}




#endif // !APP_CPP
