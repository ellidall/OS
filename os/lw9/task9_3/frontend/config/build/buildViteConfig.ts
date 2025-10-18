import {UserConfig} from 'vite'
import {buildBuild} from './buildBuild'
import {buildPlugins} from './buildPlugins'
import {buildResolve} from './buildResolve'
import {buildServer} from './buildServer'
import {BuildOptions} from './types/config'

function buildViteConfig(args: BuildOptions): UserConfig {
	const {mode} = args

	return {
		mode,
		build: buildBuild(args),
		plugins: buildPlugins(args),
		server: buildServer(args),
		resolve: buildResolve(args),
	}
}

export {
	buildViteConfig,
}