// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
package com.umicom.hello

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.material3.Text
import androidx.compose.material3.MaterialTheme

/**
 * MainActivity — smallest possible Compose host.
 * This keeps the skeleton modern and easy to extend.
 */
class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            MaterialTheme {
                Text("Hello from Umicom Studio IDE (Android)!")
            }
        }
    }
}
