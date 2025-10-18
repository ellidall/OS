type BuildMode = 'development' | 'production'

type BuildPaths = {
    html: string,
    entry: string,
    build?: string,
}

type BuildAliases = Record<string, string>

type BuildOptions = {
    port: number,
    isDev: boolean,
    mode: BuildMode,
    paths: BuildPaths,
    aliases: BuildAliases,
}

export type {
	BuildMode,
	BuildPaths,
	BuildAliases,
	BuildOptions,
}