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

  // Update projection matrix;
  _renderer->update_projection(aspect);
  // Info_Print(std::to_string(width)+"x"+std::to_string(height));
}


static void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
  App* app = static_cast<App *>(glfwGetWindowUserPointer(window));
  if (app) {
    app->mouse_cursor_event(xpos, -ypos);
  } else {
    Err_Print("Cannot get the app ptr during callback", "app.cpp");
  }
}
void App::mouse_cursor_event(double xpos, double ypos)
{
  if (camera_is_moving and mouse_on_viewport)
  {
    vec2 mousepos = vec2(float(xpos), float(-ypos));

    if (firstMouse)
    {
      _lastmousepos = mousepos+vec2(0.0001f, 0.00001f);
      firstMouse = false  ;
    }                     
    vec2 diff = mousepos-_lastmousepos;
    _lastmousepos = mousepos;
    if (rot and !(mov or scale))
    {
    float angle = length(diff) * sensi_rot;
    _renderer->update_rotation(diff, angle);
    // if (rotationAxis[0]!=rotationAxis[0]) {
    //   Warn_Print("NAN in rotationAxis vector computation for camera view");
    //   rotationAxis = glm::vec3(0.0f,0.0f,1.0f);
    // }
    }
    if (mov)
    {
      _renderer->update_camerapos(sensi_mov * diff);
    }
    if (scale)
    {
      float dist = (diff[1]<0) ? sensi_scale * length(diff) :  -sensi_scale * length(diff);
      _renderer->update_cameradist(dist);
    }
  }
}

static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  App* app = static_cast<App *>(glfwGetWindowUserPointer(window));
  if (app) {
    app->mouse_scroll_event(yoffset);
  } else {
    Err_Print("Cannot get the app ptr during callback", "app.cpp");
  }
}
void App::mouse_scroll_event(double yoffset)
{
  if (mouse_on_viewport) {
    float dist = sensi_scale * float(-yoffset);
    _renderer->update_cameradist(dist);
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

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    _timeline.play_button();
}

// Constructor
App::App(Renderer* renderer, Skeleton* skeleton, MuscleSystem* muscles) :
  _renderer(renderer), /*_simulator(),*/ _skel(skeleton), _musc(muscles),
  size_window(1.7f), show_demo_window(true),  // GLFW variable

  sensi_rot(0.3f), sensi_mov(0.0007f), sensi_scale(0.5f),                  // Camera control variable
  mouse_on_viewport(false), camera_is_moving(false), rot(false), mov(false), scale(false),
  firstMouse(true), lastframe(0),
  img_size(1.0f), pannel_size(vec2(1191.0f, 819.0f)),
  is_simulating(false), _swap_interval(3)
{
  _timeline.set_last_frame(_skel->_nb_frames);
  Title_Print("Launch Simuscle App (with skeleton)");
  Info_Print("Commit  : ******");
  Info_Print("Modif   : 23/09/23");
  Info_Print("Version : 0.0.0");
}

void App::Init()
{
  /******** Init OpenGL ********/
  Title_Print("Initialisation");
  Info_Print("Init GLFW window");
  glfwInit();
  // Choose GLSL version 
  Info_Print("Set GLSL version");
  const char* glsl_version = "#version 420";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  // Create window with graphics context
  Info_Print("Create GLFW window");
  window = glfwCreateWindow(size_window*1280, size_window*720, "Simuscle", nullptr, nullptr);
  if (window == nullptr) {
    Err_Print("Failed to launch GLFW window", "main.cpp");
    exit(1);
  }

  // Parameters
  Info_Print("Set GLFW parameters");
  glfwMakeContextCurrent(window);
  // Link callback function
  glfwSetWindowUserPointer(window, this);
  // glfwSetFramebufferSizeCallback(window, window_size_callback);
  glfwSetScrollCallback(window, mouse_scroll_callback);
  glfwSetCursorPosCallback(window, mouse_cursor_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  // Enable vsync
  // glfwSwapInterval(0); // number of screen update to wait before sending render to the screen
  glfwSwapInterval(_swap_interval);
  // Load extension's functions
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

  /******** Geometry Init ********/

  /******** Render Init *******/
  Title_Print("Init Renderer");
  _renderer->Init(int(pannel_size.x), int(pannel_size.y));

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

  // To get the 1st render window:
  window_resize_event(1280, 720);
  Info_Print("Initialisation Done");
}


void App::Run()
{
  Title_Print("Run");
  Info_Print("Running...");
  while (!glfwWindowShouldClose(window))
  {
    Info_Print("");
    Title_Print("New frame");
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
    // bool b=true;
    // bool *p_open;
    // p_open = &b;
    // static bool opt_fullscreen = true;
    // static bool opt_padding = false;
    // static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // // because it would be confusing to have two docking targets within each others.
    // ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    // const ImGuiViewport* viewport = ImGui::GetMainViewport();
    // ImGui::SetNextWindowPos(viewport->WorkPos);
    // ImGui::SetNextWindowSize(viewport->WorkSize);
    // ImGui::SetNextWindowViewport(viewport->ID);
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    // window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    // window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // // and handle the pass-thru hole, so we ask Begin() to not render a background.
    // if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    //     window_flags |= ImGuiWindowFlags_NoBackground;

    // // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // // all active windows docked into it will lose their parent and become undocked.
    // // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    // ImGui::Begin("Viewport", p_open, window_flags);
    // ImGui::PopStyleVar();
    // ImGui::PopStyleVar(2);

    // // Submit the DockSpace
    // ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    // ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    // dockspace_flags = ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoResize;

    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("Options"))
      {
        // Disabling fullscreen would allow the window to be moved to the front of other windows,
        // which we can't undo at the moment without finer window depth/z control.
        bool opt_padding = true;
        ImGui::MenuItem("Padding", NULL, &opt_padding);
        ImGui::Separator();
        ImGui::Text("C'est le text du padding");
        ImGui::Separator();

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

      ImGui::EndMainMenuBar();
    }
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    /********* Viewport ********/ 
    bool p_open = true;
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    ImGui::Begin("Viewport", &p_open, window_flags);
    mouse_on_viewport = ImGui::IsWindowHovered();
    // resize_fbo
    ImVec2 vp_size = ImGui::GetContentRegionAvail();
    if (vp_size.x != pannel_size.x or vp_size.y != pannel_size.y)
    {
      Info_Print("Resize viewport to the new value: "+std::to_string(int(vp_size.x))+"x"+std::to_string(int(vp_size.y)));
      _renderer->resize_fbo(vp_size.x, vp_size.y);
      window_resize_event(int(vp_size.x), int(vp_size.y));
      pannel_size.x = vp_size.x;
      pannel_size.y = vp_size.y;
    }
    unsigned int FrameBufferID = _renderer->textureColorbuffer;
    ImGui::Image((void *) FrameBufferID, ImVec2(pannel_size.x, pannel_size.y));
    ImGui::End();


    /********* UI Pannels ********/
    ImGui::ShowDemoWindow();
    UI_control_pannel(io);
    _timeline.UI_pannel();
    _renderer->UI_pannel();
    _skel->UI_pannel();
    _musc->UI_pannel();

    /********* Timeline ********/
    _timeline.time_step();


    /********* Physics Solver ********/
    if (lastframe!=_timeline.get_frame())
    _skel->update_buffers(_timeline.get_frame());
    if (is_simulating)
      _musc->solve(_timeline.get_frame());


    /******** Rendering ********/
    ImGui::Render();
    _renderer->Draw(_timeline.get_frame());
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    /******** ??????? ********/
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

    /******** Swat Buffers ********/
    glfwSwapBuffers(window);

    lastframe = _timeline.get_frame();
  }

}

void App::UI_control_pannel(ImGuiIO& io)
{
  ImGui::Begin("Camera control");                          // Create a window called "Hello, world!" and append into it.

  ImGui::Text("Rotation sensitivity:");
  ImGui::SliderFloat(" ", &sensi_rot, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
  ImGui::Text("Movement sensitivity:");
  ImGui::SliderFloat("  ", &sensi_mov, 0.0f, 0.005f);            // Edit 1 float using a slider from 0.0f to 1.0f
  ImGui::Text("Zoom sensitivity:");
  ImGui::SliderFloat("   ", &sensi_scale, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
  ImGui::Separator();
  std::string msg;
  for (int i = 0; i<4*4; i++) {
    msg+= std::to_string(_renderer->_rotation[i%4][i/4]) + " ";
    if (i%4==3) {
      msg+= "\n ";
    }
  }
  char* char_array = new char[msg.length() + 1]; 
  strcpy(char_array, msg.c_str()); 
  ImGui::Text(msg.c_str());

  if (ImGui::Button("Reset view"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    _renderer->reset_view();

  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
  ImGui::SeparatorText("Basic parameters");
  ImGui::Checkbox("Simulate muscles physics", &is_simulating);
  ImGui::PushItemWidth(100);
  if (ImGui::InputInt("Swap interval", &_swap_interval))
    glfwSwapInterval(_swap_interval);

  ImGui::End();
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
