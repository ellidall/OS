import {BuildOptions as ViteBuildOptions} from 'vite'
import {BuildOptions} from './types/config'

function buildBuild(args: BuildOptions): ViteBuildOptions {
	const {paths} = args

	return {
		outDir: paths.build,
		rollupOptions: {
			input: {
				app: paths.entry,
			},
			output: {
				dir: paths.build,
			},
		},
	}
}

export {
	buildBuild,
}