use ash::{
    version::{EntryV1_0, InstanceV1_0},
    vk, Entry,
};
use std::ffi::CString;

pub struct Context {
    entry: Entry,
    instance: ash::Instance,
}

impl Default for Context {
    fn default() -> Self {
        Context::new("solaris")
    }
}

impl Context {
    pub fn new(app_name: &str) -> Self {
        let entry = unsafe { Entry::new().unwrap() };

        let app_name = CString::new(app_name).unwrap();
        let engine_name = CString::new("MyEngine").unwrap();

        let app_info = vk::ApplicationInfo {
            p_application_name: app_name.as_ptr(),
            application_version: vk::make_version(0, 1, 0),
            p_engine_name: engine_name.as_ptr(),
            engine_version: vk::make_version(0, 1, 0),
            api_version: vk::make_version(1, 1, 0),
            ..Default::default()
        };

        let layer_names: Vec<*const i8> = vec![];
        let extension_names: Vec<*const i8> = vec![];

        let instance_create_info = vk::InstanceCreateInfo {
            p_application_info: &app_info,
            enabled_layer_count: layer_names.len() as u32,
            pp_enabled_layer_names: if layer_names.is_empty() {
                std::ptr::null()
            } else {
                layer_names.as_ptr()
            },
            enabled_extension_count: extension_names.len() as u32,
            pp_enabled_extension_names: if extension_names.is_empty() {
                std::ptr::null()
            } else {
                extension_names.as_ptr()
            },
            ..Default::default()
        };

        let instance = unsafe {
            entry
                .create_instance(&instance_create_info, None)
                .expect("Failed to create Vulkan instance")
        };

        Self { entry, instance }
    }
}

impl Drop for Context {
    fn drop(&mut self) {
        unsafe { self.instance.destroy_instance(None) };
    }
}
