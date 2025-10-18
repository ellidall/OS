import {createSlice, PayloadAction} from '@reduxjs/toolkit'
import {DocumentType} from 'shared/types'

type DocumentsSliceState = {
    documents: DocumentType[],
	currentUrl: string,
}

const initialState: DocumentsSliceState = {
	documents: [],
	currentUrl: '',
}

const mainSlice = createSlice({
	name: 'searchResults',
	initialState,
	reducers: {
		setDocuments: (state, action: PayloadAction<DocumentType[]>) => {
			state.documents = action.payload
		},
		serUrl: (state, action: PayloadAction<string>) => {
			state.currentUrl = action.payload
		},
	},
})

export {
	mainSlice,
}