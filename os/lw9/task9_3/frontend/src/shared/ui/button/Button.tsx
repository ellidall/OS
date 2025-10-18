import {ButtonHTMLAttributes, DetailedHTMLProps} from 'react'
import {joinClasses} from '../../libs'
import styles from './Button.module.css'

type Props = DetailedHTMLProps<ButtonHTMLAttributes<HTMLButtonElement>, HTMLButtonElement> & {
    disabled?: boolean,
    className?: string,
}

const Button = (props: Props) =>
	<button {...props} className={joinClasses(styles.button, props.className)} ></button>

export {
	Button,
}