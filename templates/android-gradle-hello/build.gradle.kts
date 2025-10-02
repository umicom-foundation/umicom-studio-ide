// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
plugins { id("com.android.application") version "8.5.0" apply true }
android {
  namespace = "com.umicom.hello"
  compileSdk = 34
  defaultConfig {
    applicationId = "com.umicom.hello"
    minSdk = 24
    targetSdk = 34
    versionCode = 1
    versionName = "1.0"
  }
}
