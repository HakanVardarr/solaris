use super::winit::*;
use solaris_graphics::context::Context;
use winit::application::ApplicationHandler;
use winit::event::WindowEvent;
use winit::event_loop::ActiveEventLoop;
use winit::window::WindowAttributes;

pub trait AppLogic {
    fn update(&mut self);
    fn render(&mut self, window: &Window);
}

pub struct Application<T: AppLogic> {
    window: Option<Window>,
    context: Option<Context>,
    logic: T,
    app_name: &'static str,
}

impl<T: AppLogic> Application<T> {
    pub fn new(logic: T, app_name: &'static str) -> Self {
        Self {
            window: None,
            context: None,
            logic,
            app_name,
        }
    }

    fn init_window(&mut self, event_loop: &ActiveEventLoop) {
        self.window = Some(
            event_loop
                .create_window(WindowAttributes::default().with_title(self.app_name))
                .unwrap(),
        );
    }

    fn init_vulkan(&mut self) {
        self.context = Some(Context::new(self.app_name))
    }
}

impl<T: AppLogic> ApplicationHandler for Application<T> {
    fn resumed(&mut self, event_loop: &ActiveEventLoop) {
        self.init_window(event_loop);
        self.init_vulkan();
    }

    fn window_event(
        &mut self,
        event_loop: &ActiveEventLoop,
        _window_id: winit::window::WindowId,
        event: WindowEvent,
    ) {
        match event {
            WindowEvent::CloseRequested => {
                event_loop.exit();
            }
            WindowEvent::RedrawRequested => {
                if let Some(window) = &self.window {
                    self.logic.update();
                    self.logic.render(window);
                    window.request_redraw(); // sürekli redraw için
                }
            }
            _ => {}
        }
    }
}
