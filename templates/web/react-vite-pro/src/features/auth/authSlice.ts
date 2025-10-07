import { createSlice } from '@reduxjs/toolkit';
const slice = createSlice({ name:'auth', initialState:{ token:null as string|null }, reducers:{ setToken(s,a){ s.token=a.payload }, clear(s){ s.token=null } }});
export const { setToken, clear } = slice.actions; export default slice.reducer;
