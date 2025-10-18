import {ReactNode} from 'react'
import {HeaderWidget} from 'widgets/header'
import styles from './Styles.module.css'

export const withLayout = (component: ReactNode, headerTitle: string) =>
	<>
		<HeaderWidget title={headerTitle}/>
		<div className={styles.container}>
			<div className={styles.content}>
				{component}
			</div>
		</div>
	</>