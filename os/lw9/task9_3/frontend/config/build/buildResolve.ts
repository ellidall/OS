import {AliasOptions, ResolveOptions} from 'vite'
import {BuildOptions} from './types/config'

function buildResolve(args: BuildOptions): ResolveOptions & {alias?: AliasOptions} {
	const {aliases} = args

	return {
		alias: aliases,
	}
}

export {
	buildResolve,
}