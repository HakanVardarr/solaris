use std::collections::BTreeMap;

use ash::{
    vk::{PhysicalDevice, PhysicalDeviceType},
    Instance,
};

#[derive(Debug)]
pub struct Device {
    _inner: PhysicalDevice,
}

impl Device {
    pub fn choose_suitable_device(
        instance: &Instance,
        devices: Vec<PhysicalDevice>,
    ) -> anyhow::Result<Device> {
        if devices.is_empty() {
            return Err(anyhow::Error::msg("Failed to find GPUs on device."));
        }

        let mut candidates = BTreeMap::<u32, PhysicalDevice>::new();
        for device in devices.iter() {
            let score = unsafe { Self::rate_device_suitability(instance, device) };
            candidates.insert(score, *device);
        }

        if let Some((_score, device)) = candidates.iter().next_back() {
            Ok(Self { _inner: *device })
        } else {
            Err(anyhow::Error::msg("failed to find a suitable GPU!"))
        }
    }

    unsafe fn rate_device_suitability(instance: &Instance, device: &PhysicalDevice) -> u32 {
        let mut score = 0;

        let properties = instance.get_physical_device_properties(*device);
        let _features = instance.get_physical_device_features(*device);

        if properties.device_type == PhysicalDeviceType::DISCRETE_GPU {
            score += 1000;
        }
        score += properties.limits.max_image_dimension2_d;

        score
    }

    unsafe fn is_device_suitable(instance: &Instance, device: &PhysicalDevice) -> bool {
        let properties = instance.get_physical_device_properties(*device);
        let features = instance.get_physical_device_features(*device);

        properties.device_type == PhysicalDeviceType::DISCRETE_GPU
    }
}
