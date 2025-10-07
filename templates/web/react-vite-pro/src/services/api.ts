import { createApi, fetchBaseQuery } from '@reduxjs/toolkit/query/react';
export const api = createApi({ reducerPath:'api', baseQuery: fetchBaseQuery({ baseUrl:'/api' }), endpoints: (b)=> ({ hello: b.query<{ok:boolean,message:string}, void>({ query: ()=> '/hello' }) }) });
export const { useHelloQuery } = api;
