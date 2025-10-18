import {MethodType} from 'shared/types'

export const fetchData = async (url: string, method: MethodType, params?: any) => {
	const data: any = {
		method: method,
		headers: {
			'Content-Type': 'application/json',
		},
	}

	if (method === 'POST') {
		data.body = JSON.stringify(params || {})
	}

	const response = await fetch(url, data)

	/*let result
	response.json()
		.then(res => {
			result = res
		})
		.catch(err => alert('Ошибка: ' + err))*/

	return await response.json()
}