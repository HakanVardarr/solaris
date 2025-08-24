use thiserror::Error;

#[derive(Debug, Error)]
pub enum ContextError {
    #[error("Failed to find layer named: {0}")]
    ValidationLayerSupport(String),
}
