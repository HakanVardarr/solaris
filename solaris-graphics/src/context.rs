use ash::{vk, Entry};
use once_cell::sync::Lazy;
use std::ffi::{c_char, CString};
use winit::{raw_window_handle::HasDisplayHandle, window::Window};

static VALIDATION_LAYERS: Lazy<Vec<CString>> =
    Lazy::new(|| vec![CString::new("VK_LAYER_KHRONOS_validation").unwrap()]);

pub struct Context {
    _entry: Entry,
    instance: ash::Instance,
}

impl Context {
    pub fn new(app_name: &str, window: &Window) -> anyhow::Result<Self> {
        let entry = unsafe { Entry::load()? };

        let app_name = CString::new(app_name)?;
        let engine_name = CString::new("solaris")?;

        let app_info = vk::ApplicationInfo {
            p_application_name: app_name.as_ptr(),
            application_version: vk::make_api_version(0, 1, 0, 0),
            p_engine_name: engine_name.as_ptr(),
            engine_version: vk::make_api_version(0, 1, 0, 0),
            api_version: vk::make_api_version(1, 1, 0, 0),
            ..Default::default()
        };

        let extensions = Context::get_required_extensions(window);
        #[cfg(debug_assertions)]
        let layer_names: Vec<*const i8> = VALIDATION_LAYERS.iter().map(|c| c.as_ptr()).collect();
        #[cfg(not(debug_assertions))]
        let layer_names: Vec<*const i8> = Vec::new();

        #[allow(unused_mut)]
        let mut instance_create_info = vk::InstanceCreateInfo {
            p_application_info: &app_info,
            enabled_layer_count: layer_names.len() as u32,
            pp_enabled_layer_names: if layer_names.is_empty() {
                std::ptr::null()
            } else {
                layer_names.as_ptr()
            },
            enabled_extension_count: extensions.len() as u32,
            pp_enabled_extension_names: extensions.as_ptr(),
            ..Default::default()
        };

        #[cfg(target_os = "macos")]
        {
            instance_create_info.flags |= vk::InstanceCreateFlags::ENUMERATE_PORTABILITY_KHR;
        }

        let instance = unsafe { entry.create_instance(&instance_create_info, None)? };

        Ok(Self {
            _entry: entry,
            instance,
        })
    }

    fn get_required_extensions(window: &Window) -> Vec<*const c_char> {
        let handle = window.display_handle().unwrap();
        let display_handle = handle.as_raw();

        let mut extensions = ash_window::enumerate_required_extensions(display_handle)
            .unwrap()
            .to_vec();

        #[cfg(debug_assertions)]
        extensions.push(ash::ext::debug_utils::NAME.as_ptr());

        #[cfg(target_os = "macos")]
        extensions.push(ash::vk::KHR_PORTABILITY_ENUMERATION_NAME);

        extensions
    }
}

impl Drop for Context {
    fn drop(&mut self) {
        unsafe { self.instance.destroy_instance(None) };
    }
}
