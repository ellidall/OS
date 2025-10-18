import {DetailedHTMLProps, InputHTMLAttributes, TextareaHTMLAttributes} from 'react'
import {joinClasses} from '../../libs'
import styles from './Input.module.css'

type InputProps = DetailedHTMLProps<InputHTMLAttributes<HTMLInputElement>, HTMLInputElement> & {
    className?: string,
}

type TextareaProps =  DetailedHTMLProps<TextareaHTMLAttributes<HTMLTextAreaElement>, HTMLTextAreaElement> & {
    className?: string,
}

const Input = (props: InputProps) =>
	<input type={"text"} {...props} className={joinClasses(styles.input, props.className)}></input>

const Textarea = (props: TextareaProps) =>
    <textarea {...props} className={joinClasses(styles.textarea, props.className)}></textarea>

export {
    Input,
    Textarea,
}