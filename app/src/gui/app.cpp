#include "app.hpp"
#include <implot.h>
#include <imnodes.h>
#include <data-flow/gui.hpp>
using namespace Magnum;

namespace gui
{
    App::App(const Arguments &arguments)
        : Platform::Application{arguments, Configuration{}.setTitle("GNSS Tester").setWindowFlags(Configuration::WindowFlag::Resizable)},
          device_create_modal_{device_manager_},
          test_data_flow_editor_{nullptr}
    {

        ImGui::CreateContext();
        ImPlot::CreateContext();

        dt::df::InitDataflow(ImGui::GetCurrentContext());
        auto &imgui_io = ImGui::GetIO();
        {
            imnodes::Initialize();
            imnodes::IO &imnodes_io = imnodes::GetIO();
            imnodes_io.link_detach_with_modifier_click.modifier = &imgui_io.KeyCtrl;
        }

        imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        imgui_io.Fonts->Clear();

        const Vector2 size = Vector2{windowSize()} / dpiScaling();
        const float supersamplingRatio = framebufferSize().x() / size.x();

        auto *roboto_font = imgui_io.Fonts->AddFontFromFileTTF("fonts/Roboto/Roboto-Regular.ttf", 18.0f * supersamplingRatio); //0
        gFonts["Text"] = roboto_font;
        auto *firacode_font = imgui_io.Fonts->AddFontFromFileTTF("fonts/Iosevka/iosevka-term-regular.ttf", 18.0f * supersamplingRatio); //1
        gFonts["Monospace"] = firacode_font;
        imgui_io.FontDefault = roboto_font;

        imgui_ = ImGuiIntegration::Context(*ImGui::GetCurrentContext(), size, windowSize(), framebufferSize());

        /* Set up proper blending to be used by ImGui. There's a great chance
            you'll need this exact behavior for the rest of your scene. If not, set
            this only for the drawFrame() call. */
        GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
                                       GL::Renderer::BlendEquation::Add);
        GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
                                       GL::Renderer::BlendFunction::OneMinusSourceAlpha);

        // we don't need any other than imgui.
        GL::Renderer::enable(GL::Renderer::Feature::Blending);
        GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
        GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
                                       GL::Renderer::BlendEquation::Add);
        GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
                                       GL::Renderer::BlendFunction::OneMinusSourceAlpha);

#if !defined(MAGNUM_TARGET_WEBGL) && !defined(CORRADE_TARGET_ANDROID)
        /* Have some sane speed, please */
        setMinimalLoopPeriod(16);
#endif

        test_data_flow_editor_ = std::make_unique<DataFlowEditor>(std::string{"TEST"}, protocol_loader_);
    }

    void App::drawEvent()
    {
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

        imgui_.newFrame();

        /* Enable text input, if needed */
        if (ImGui::GetIO().WantTextInput && !isTextInputActive())
            startTextInput();
        else if (!ImGui::GetIO().WantTextInput && isTextInputActive())
            stopTextInput();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        showMainMenu();
        device_create_modal_.draw();
        device_manager_.draw();
        if (test_data_flow_editor_)
            test_data_flow_editor_->draw();
        ImGui::ShowDemoWindow();

        /* Update application cursor */
        imgui_.updateApplicationCursor(*this);

        imgui_.drawFrame();

        swapBuffers();
        redraw();
    }

    void App::showMainMenu()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Device"))
            {
                showDeviceMenu();
                ImGui::EndMenu();
            }
            device_manager_.drawMenu();
            ImGui::EndMainMenuBar();
        }
    }

    void App::showDeviceMenu()
    {
        if (ImGui::MenuItem("Open", "Ctrl+O"))
        {
            device_create_modal_.open();
        }
        if (ImGui::BeginMenu("Open Recent"))
        {
            device_manager_.drawOpenRecent();
            ImGui::EndMenu();
        }
    }

    void App::viewportEvent(ViewportEvent &event)
    {
        GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
        imgui_.relayout(Vector2{event.windowSize()} / event.dpiScaling(), event.windowSize(), event.framebufferSize());
    }

    void App::keyPressEvent(KeyEvent &event)
    {
        if (imgui_.handleKeyPressEvent(event))
            return;
    }
    void App::keyReleaseEvent(KeyEvent &event)
    {
        if (imgui_.handleKeyReleaseEvent(event))
            return;
    }

    void App::mousePressEvent(MouseEvent &event)
    {
        if (imgui_.handleMousePressEvent(event))
            return;
    }
    void App::mouseReleaseEvent(MouseEvent &event)
    {
        if (imgui_.handleMouseReleaseEvent(event))
            return;
    }
    void App::mouseMoveEvent(MouseMoveEvent &event)
    {
        if (imgui_.handleMouseMoveEvent(event))
            return;
    }
    void App::mouseScrollEvent(MouseScrollEvent &event)
    {
        if (imgui_.handleMouseScrollEvent(event))
        {
            /* Prevent scrolling the page */
            event.setAccepted();
            return;
        }
    }
    void App::textInputEvent(TextInputEvent &event)
    {
        if (imgui_.handleTextInputEvent(event))
            return;
    }

    App::~App()
    {
        ImPlot::DestroyContext();
        imnodes::Shutdown();
        dt::df::ShutdownDataflow();
    }
} // namespace gui
