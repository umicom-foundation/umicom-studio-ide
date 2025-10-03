// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
// Settings for Android project using Gradle Kotlin DSL.
pluginManagement {
    repositories {
        google()
        mavenCentral()
        gradlePluginPortal()
    }
}
dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
    }
}
rootProject.name = "UmicomAndroidHello"
include(":app")
