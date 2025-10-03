// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Hilt module providing a fake API service (replace with Retrofit).


package com.umicom.enterprise.di
import dagger.Module; import dagger.Provides; import dagger.hilt.InstallIn; import dagger.hilt.components.SingletonComponent
import javax.inject.Singleton
interface Api { fun hello(): String }
class FakeApi: Api { override fun hello() = "Hello from Api" }
@Module @InstallIn(SingletonComponent::class)
object NetworkModule { @Provides @Singleton fun provideApi(): Api = FakeApi() }
