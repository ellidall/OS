import {withLayout} from 'app/providers'
import {AddPage} from 'pages/add'
import {DocumentPage} from 'pages/document'
import {MainPage} from 'pages/main'
import {ReactNode} from 'react'
import {RouteObject} from 'react-router-dom'

const createRoute = (path: string, element: ReactNode): RouteObject => ({
	path,
	element: element,
})

const routes: RouteObject[] = [
	createRoute(
		'/*',
		withLayout(<MainPage/>, 'Поисковая система'),
	),
	createRoute(
		'/add',
		withLayout(<AddPage/>, 'Добавить документ'),
	),
	createRoute(
		'/:url',
		withLayout(<DocumentPage/>, 'Документ'),
	),
]

export {
	routes,
}