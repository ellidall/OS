import {configureStore} from '@reduxjs/toolkit'
import {mainSlice} from 'pages/model'
import {TypedUseSelectorHook, useDispatch, useSelector} from 'react-redux'

const store = configureStore({
	reducer: {
		documents: mainSlice.reducer,
	},
})

type AppDispatch = typeof store.dispatch
type RootState = ReturnType<typeof store.getState>
const useAppDispatch: () => AppDispatch = useDispatch
const useAppSelector: TypedUseSelectorHook<RootState> = useSelector

export {
	store,
	useAppDispatch,
	useAppSelector,
}