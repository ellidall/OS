import styles from './HeaderWidget.module.css'

type Props = {
    title: string,
}

const HeaderWidget = ({title}: Props) =>
	<div className={styles.header}>
		<div className={styles.headerContent}>
			<span className={styles.title}>
				{title}
			</span>
		</div>
	</div>

export {
	HeaderWidget,
}