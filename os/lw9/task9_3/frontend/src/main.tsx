import {withStore} from 'app/providers'
import {routes} from 'app/routes'
import 'app/styles/index.css'
import ReactDOM from 'react-dom/client'
import {createBrowserRouter, RouterProvider} from 'react-router-dom'

// eslint-disable-next-line @typescript-eslint/no-non-null-assertion
ReactDOM.createRoot(document.getElementById('root')!).render(
	withStore(<RouterProvider router={createBrowserRouter(routes)}/>),
)