import { configureStore } from '@reduxjs/toolkit';
import { api } from '../services/api';
import auth from '../features/auth/authSlice';
export const store = configureStore({ reducer: { [api.reducerPath]: api.reducer, auth }, middleware: g => g().concat(api.middleware) });
export type RootState = ReturnType<typeof store.getState>;
