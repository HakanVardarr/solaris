use solaris_core::application::{AppLogic, Application};
use solaris_core::winit::{EventLoop, Window};

struct Game;

impl AppLogic for Game {
    fn render(&mut self, _window: &Window) {}
    fn update(&mut self) {}
}

fn main() {
    let event_loop = EventLoop::new().unwrap();
    let mut application = Application::new(Game, "sandbox");
    event_loop.run_app(&mut application).unwrap();
}
