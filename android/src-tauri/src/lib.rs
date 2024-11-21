use tauri_plugin_http::reqwest;

// Learn more about Tauri commands at https://tauri.app/develop/calling-rust/
#[tauri::command]
async fn fetch_url(url: String, method: String) -> String {
    // Perform the GET request using reqwest
    let client = reqwest::Client::new();
    if method == "POST" {
        match client.post(&url).send().await {
            Ok(response) => {
                if response.status().is_success() {
                    // Read and return the response body
                    match response.text().await {
                        Ok(body) => body,
                        Err(_err) => "Nope".to_string(),
                    }
                } else {
                    "Nope".to_string()
                }
            }
            Err(_err) => "Nope".to_string(),
        }
    } else {
        match client.get(&url).send().await {
            Ok(response) => {
                if response.status().is_success() {
                    // Read and return the response body
                    match response.text().await {
                        Ok(body) => body,
                        Err(_err) => "Nope".to_string(),
                    }
                } else {
                    "Nope".to_string()
                }
            }
            Err(err) => "Nope".to_string(),
        }
    }
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_http::init())
        .plugin(tauri_plugin_shell::init())
        .invoke_handler(tauri::generate_handler![fetch_url])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
