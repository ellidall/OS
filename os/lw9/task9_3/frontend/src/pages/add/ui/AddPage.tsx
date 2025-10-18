import {useState} from 'react'
import {useNavigate} from 'react-router-dom'
import {SERVER_URL, URL_ADD, URL_MAIN} from 'shared/consts'
import {DocumentType} from 'shared/types'
import {Button} from 'shared/ui/button'
import {Input, Textarea} from 'shared/ui/input'


const AddPage = () => {
	const navigate = useNavigate()
	const [document, setDocument] = useState<DocumentType>(
		{
			url:'',
			title: '',
			content: '',
		},
	)

	const handleEditDocument = async () => {
		try {
			const res = await fetch(SERVER_URL + URL_ADD, {
				method: 'POST',
				headers: {
					'Content-Type': 'application/json',
				},
				body: JSON.stringify({
					url: document.url,
					title: document.title,
					content: document.content,
				}),
			})

			if (res.ok) {
				alert('Document added successfully')
			}
			else {
				alert('Error: Failed to add document')
			}
		}
		catch (error) {
			alert('Error: ' + error)
		}
		navigate(URL_MAIN)
	}

	return (
		<div>
			<Input
				placeholder={'Url'}
				value={document.url}
				onChange={e => setDocument({...document, url: e.target.value})}
			/>
			<Input
				placeholder={'Title'}
				value={document.title}
				onChange={e => setDocument({...document, title: e.target.value})}
			/>
			<Textarea
				placeholder={'Content'}
				value={document.content}
				onChange={e => setDocument({...document, content: e.target.value})}
			/>
			<Button onClick={handleEditDocument}>{'Add Document'}</Button>
			<Button onClick={() => navigate(URL_MAIN)}>{'Back'}</Button>
		</div>
	)
}

export {
	AddPage,
}