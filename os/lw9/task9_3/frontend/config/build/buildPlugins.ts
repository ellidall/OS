import react from '@vitejs/plugin-react'
import {PluginOption} from 'vite'
// import symfonyPlugin from 'vite-plugin-symfony'
import {BuildOptions} from './types/config'

function buildPlugins(args: BuildOptions): PluginOption[] {
	const {isDev} = args

	return [
		react(),
		// isDev ? false : symfonyPlugin(),
	]
}

export {
	buildPlugins,
}