use ash::vk;
use tracing::info;
use winit::raw_window_handle::{HasDisplayHandle, HasWindowHandle};

pub struct Surface {
    surface: vk::SurfaceKHR,
    loader: ash::khr::surface::Instance,
}

impl Surface {
    pub fn new(
        window: &winit::window::Window,
        entry: &ash::Entry,
        instance: &ash::Instance,
    ) -> anyhow::Result<Surface> {
        let display_handle = window.display_handle()?;
        let window_handle = window.window_handle()?.as_raw();

        let surface = unsafe {
            ash_window::create_surface(
                entry,
                instance,
                display_handle.as_raw(),
                window_handle,
                None,
            )
        }?;
        let loader = ash::khr::surface::Instance::new(entry, instance);
        info!("Window surface created successfully.");

        Ok(Self { surface, loader })
    }
}

impl Drop for Surface {
    fn drop(&mut self) {
        unsafe { self.loader.destroy_surface(self.surface, None) };
    }
}
