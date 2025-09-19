use ash::{ext::debug_utils, vk};
use std::ffi::CStr;
use tracing::{debug, error, info, warn};

pub struct DebugMessenger {
    messenger: vk::DebugUtilsMessengerEXT,
    loader: debug_utils::Instance,
}

impl DebugMessenger {
    pub fn new(
        entry: &ash::Entry,
        instance: &ash::Instance,
    ) -> anyhow::Result<Option<DebugMessenger>> {
        #[cfg(not(debug_assertions))]
        return Ok(None);

        let debug_messenger_create_info = vk::DebugUtilsMessengerCreateInfoEXT {
            message_severity: vk::DebugUtilsMessageSeverityFlagsEXT::ERROR
                | vk::DebugUtilsMessageSeverityFlagsEXT::WARNING
                | vk::DebugUtilsMessageSeverityFlagsEXT::VERBOSE,
            message_type: vk::DebugUtilsMessageTypeFlagsEXT::PERFORMANCE
                | vk::DebugUtilsMessageTypeFlagsEXT::VALIDATION
                | vk::DebugUtilsMessageTypeFlagsEXT::GENERAL,
            pfn_user_callback: Some(debug_callback),
            ..Default::default()
        };
        let debug_utils_loader = debug_utils::Instance::new(entry, instance);

        Ok(Some(DebugMessenger {
            messenger: unsafe {
                debug_utils_loader.create_debug_utils_messenger(&debug_messenger_create_info, None)
            }?,
            loader: debug_utils_loader,
        }))
    }
}

impl Drop for DebugMessenger {
    fn drop(&mut self) {
        unsafe {
            self.loader
                .destroy_debug_utils_messenger(self.messenger, None)
        };
    }
}

unsafe extern "system" fn debug_callback(
    message_severity: vk::DebugUtilsMessageSeverityFlagsEXT,
    message_type: vk::DebugUtilsMessageTypeFlagsEXT,
    callback_data: *const vk::DebugUtilsMessengerCallbackDataEXT<'_>,
    _user_data: *mut core::ffi::c_void,
) -> vk::Bool32 {
    let callback_data = *callback_data;

    let message_id = callback_data.message_id_number;
    let message_name = if !callback_data.p_message_id_name.is_null() {
        CStr::from_ptr(callback_data.p_message_id_name).to_string_lossy()
    } else {
        std::borrow::Cow::Borrowed("NoName")
    };

    let message = if !callback_data.p_message.is_null() {
        CStr::from_ptr(callback_data.p_message).to_string_lossy()
    } else {
        std::borrow::Cow::Borrowed("")
    };

    let mut types = Vec::new();
    if message_type.contains(vk::DebugUtilsMessageTypeFlagsEXT::GENERAL) {
        types.push("General");
    }
    if message_type.contains(vk::DebugUtilsMessageTypeFlagsEXT::VALIDATION) {
        types.push("Validation");
    }
    if message_type.contains(vk::DebugUtilsMessageTypeFlagsEXT::PERFORMANCE) {
        types.push("Performance");
    }
    let type_str = types.join("|");

    if message_severity.contains(vk::DebugUtilsMessageSeverityFlagsEXT::ERROR) {
        error!(target: "vulkan", "[{}] {} - {}: {}", type_str, message_id, message_name, message);
    } else if message_severity.contains(vk::DebugUtilsMessageSeverityFlagsEXT::WARNING) {
        warn!(target: "vulkan", "[{}] {} - {}: {}", type_str, message_id, message_name, message);
    } else if message_severity.contains(vk::DebugUtilsMessageSeverityFlagsEXT::INFO) {
        info!(target: "vulkan", "[{}] {} - {}: {}", type_str, message_id, message_name, message);
    } else if message_severity.contains(vk::DebugUtilsMessageSeverityFlagsEXT::VERBOSE) {
        debug!(target: "vulkan", "[{}] {} - {}: {}", type_str, message_id, message_name, message);
    }

    vk::FALSE
}
