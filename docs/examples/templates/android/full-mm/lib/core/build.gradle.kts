// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Kotlin Android library with a small composable.


plugins {
  id("com.android.library") version "8.5.0"
  id("org.jetbrains.kotlin.android") version "1.9.24"
}
android {
  namespace = "com.umicom.core"
  compileSdk = 34
  defaultConfig { minSdk = 24 }
  buildFeatures { compose = true }
  composeOptions { kotlinCompilerExtensionVersion = "1.5.14" }
  compileOptions { sourceCompatibility = JavaVersion.VERSION_17; targetCompatibility = JavaVersion.VERSION_17 }
  kotlinOptions { jvmTarget = "17" }
}
dependencies {
  implementation(platform("androidx.compose:compose-bom:2024.06.00"))
  implementation("androidx.compose.ui:ui")
  implementation("androidx.compose.material3:material3")
}
