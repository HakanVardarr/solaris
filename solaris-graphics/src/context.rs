use crate::debug_messenger::DebugMessenger;
use crate::device::Device;
use crate::error::ContextError;
use crate::surface::Surface;

use ash::{vk, Entry};
use once_cell::sync::Lazy;
use std::ffi::{c_char, CStr, CString};
use tracing::info;
use winit::{raw_window_handle::HasDisplayHandle, window::Window};

static VALIDATION_LAYERS: Lazy<Vec<CString>> =
    Lazy::new(|| vec![CString::new("VK_LAYER_KHRONOS_validation").unwrap()]);

pub struct Context {
    _entry: Entry,
    instance: ash::Instance,

    _app_name: CString,
    _engine_name: CString,

    debug_messenger: Option<DebugMessenger>,
    surface: Option<Surface>,
    device: Option<Device>,
}

impl Context {
    pub fn new(app_name: &str, window: &Window) -> anyhow::Result<Self> {
        info!("Creating Vulkan context for app: {}", app_name);

        let entry = Self::create_entry()?;

        let app_name_cstr = CString::new(app_name)?;
        let engine_name_cstr = CString::new("solaris")?;

        let app_info = Self::create_app_info(&app_name_cstr, &engine_name_cstr);
        let extensions = Self::get_required_extensions(window);

        #[cfg(debug_assertions)]
        let layer_names: Vec<*const i8> = VALIDATION_LAYERS.iter().map(|c| c.as_ptr()).collect();
        #[cfg(not(debug_assertions))]
        let layer_names: Vec<*const i8> = Vec::new();

        Self::check_validation_layer_support(&entry, &layer_names)?;
        let instance = Self::create_instance(&entry, &app_info, &extensions, &layer_names)?;
        let debug_messenger = Self::create_debug_messenger(&entry, &instance)?;
        let surface = Some(Self::create_surface(window, &entry, &instance)?);

        let devices = unsafe { instance.enumerate_physical_devices()? };
        let device = Some(Device::choose_suitable_device(&instance, devices)?);

        Ok(Self {
            _entry: entry,
            instance,
            _app_name: app_name_cstr,
            _engine_name: engine_name_cstr,
            debug_messenger,
            surface,
            device,
        })
    }

    fn create_entry() -> anyhow::Result<Entry> {
        info!("Loading Vulkan entry");
        let entry = unsafe { Entry::load()? };
        Ok(entry)
    }

    fn create_app_info<'a>(
        app_name: &'a CString,
        engine_name: &'a CString,
    ) -> vk::ApplicationInfo<'a> {
        vk::ApplicationInfo {
            p_application_name: app_name.as_ptr(),
            application_version: vk::make_api_version(0, 1, 0, 0),
            p_engine_name: engine_name.as_ptr(),
            engine_version: vk::make_api_version(0, 1, 0, 0),
            api_version: vk::make_api_version(1, 1, 0, 0),
            ..Default::default()
        }
    }

    fn create_instance(
        entry: &Entry,
        app_info: &vk::ApplicationInfo,
        extensions: &[*const c_char],
        layers: &[*const i8],
    ) -> anyhow::Result<ash::Instance> {
        let instance_info = vk::InstanceCreateInfo {
            p_application_info: app_info,
            enabled_extension_count: extensions.len() as u32,
            pp_enabled_extension_names: extensions.as_ptr(),
            enabled_layer_count: layers.len() as u32,
            pp_enabled_layer_names: if layers.is_empty() {
                std::ptr::null()
            } else {
                layers.as_ptr()
            },
            ..Default::default()
        };

        #[cfg(target_os = "macos")]
        let instance_info = vk::InstanceCreateInfo {
            flags: vk::InstanceCreateFlags::ENUMERATE_PORTABILITY_KHR,
            ..instance_info
        };

        let instance = unsafe { entry.create_instance(&instance_info, None)? };
        info!("Created Vulkan instance");
        Ok(instance)
    }

    fn create_debug_messenger(
        entry: &Entry,
        instance: &ash::Instance,
    ) -> anyhow::Result<Option<DebugMessenger>> {
        DebugMessenger::new(entry, instance)
    }

    fn create_surface(
        window: &Window,
        entry: &Entry,
        instance: &ash::Instance,
    ) -> anyhow::Result<Surface> {
        Surface::new(window, entry, instance)
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
    fn check_validation_layer_support(
        entry: &Entry,
        layer_names: &Vec<*const i8>,
    ) -> anyhow::Result<()> {
        unsafe {
            let layers = entry.enumerate_instance_layer_properties()?;
            for layer_name in layer_names {
                let found = layers.iter().any(|&prop| {
                    prop.layer_name_as_c_str().unwrap() == CStr::from_ptr(*layer_name)
                });
                if !found {
                    return Err(ContextError::ValidationLayerSupport(
                        CStr::from_ptr(*layer_name).to_string_lossy().to_string(),
                    )
                    .into());
                }
            }
        }
        Ok(())
    }
}

impl Drop for Context {
    fn drop(&mut self) {
        unsafe {
            let _ = self.device.take();
            let _ = self.surface.take();
            let _ = self.debug_messenger.take();
            self.instance.destroy_instance(None);
        };
    }
}
