import { configureStore, createSlice, PayloadAction } from '@reduxjs/toolkit';
type CounterState = { value:number }; const initial:CounterState={ value:0 };
const counter = createSlice({ name:'counter', initialState:initial, reducers:{
  inc(s){ s.value++ }, add(s, a:PayloadAction<number>){ s.value += a.payload }
}});
export const { inc, add } = counter.actions;
export const store = configureStore({ reducer:{ counter: counter.reducer }});
export type RootState = ReturnType<typeof store.getState>;
