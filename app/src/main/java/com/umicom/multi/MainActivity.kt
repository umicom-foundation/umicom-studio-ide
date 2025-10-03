// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Compose main activity hosting two composables from feature and core modules.


package com.umicom.multi
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.material3.*
import com.umicom.core.CoreHello
import com.umicom.notes.NotesScreen

class MainActivity: ComponentActivity() {
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContent {
      MaterialTheme {
        Scaffold(topBar={ TopAppBar(title={ Text("Umicom Multi") }) }) { _ ->
          Column {
            CoreHello(text = "from core library")
            Divider()
            NotesScreen()
          }
        }
      }
    }
  }
}
