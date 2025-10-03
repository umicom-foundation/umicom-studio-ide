// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Compose UI that shows current flavor and API greeting.


package com.umicom.enterprise
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.platform.LocalContext
import dagger.hilt.android.AndroidEntryPoint
import javax.inject.Inject
import com.umicom.enterprise.di.Api
@AndroidEntryPoint
class MainActivity: ComponentActivity(){
  @Inject lateinit var api: Api
  override fun onCreate(savedInstanceState: Bundle?){
    super.onCreate(savedInstanceState)
    setContent {
      MaterialTheme {
        val tier = LocalContext.current.resources.getString(R.string.tier)
        Text("Tier: " + tier + " | API: " + api.hello())
      }
    }
  }
}
