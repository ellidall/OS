import {ServerOptions} from 'vite'
import {BuildOptions} from './types/config'

function buildServer({paths, port}: BuildOptions): ServerOptions {
	return {
		port,
		open: paths.html,
	}
}

export {
	buildServer,
}